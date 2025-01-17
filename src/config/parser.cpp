#include "config/parser.hpp"
#include "option.hpp"
#include "result.hpp"
#include "string.hpp"
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sys/stat.h>
#include <unistd.h>

Token::Token()
{
}

Token Token::ident(std::string str, int line, int column)
{
    Token t;
    t.m_type = TOKEN_IDENTIFIER;
    t.m_str = str;
    t.m_line = line;
    t.m_column = column;
    return t;
}

Token Token::num(int64_t num, int line, int column)
{
    Token t;
    t.m_type = TOKEN_NUMBER;
    t.m_int = num;
    t.m_line = line;
    t.m_column = column;
    return t;
}

Token Token::str(std::string str, int line, int column)
{
    Token t;
    t.m_type = TOKEN_STRING;
    t.m_str = str;
    t.m_line = line;
    t.m_column = column;
    return t;
}

Token Token::left_curly(int line, int column)
{
    Token t;
    t.m_type = TOKEN_LEFT_CURLY;
    t.m_line = line;
    t.m_column = column;
    return t;
}

Token Token::right_curly(int line, int column)
{
    Token t;
    t.m_type = TOKEN_RIGHT_CURLY;
    t.m_line = line;
    t.m_column = column;
    return t;
}

Token Token::ln(int line, int column)
{
    Token t;
    t.m_type = TOKEN_LINE_BREAK;
    t.m_line = line;
    t.m_column = column;
    return t;
}

Token Token::invalid()
{
    Token t;
    t.m_type = TOKEN_INVALID;
    return t;
}

TokenType Token::type() const
{
    return m_type;
}

std::string Token::str() const
{
    return m_str;
}

int64_t Token::number() const
{
    return m_int;
}

int Token::line() const
{
    return m_line;
}

int Token::column() const
{
    return m_column;
}

int Token::width()
{
    if (m_type == TOKEN_IDENTIFIER || m_type == TOKEN_STRING)
        return m_str.size();
    else if (m_type == TOKEN_LEFT_CURLY || m_type == TOKEN_RIGHT_CURLY || m_type == TOKEN_LINE_BREAK)
        return 1;
    else if (m_type == TOKEN_NUMBER)
        return to_string(m_int).size();
    return 0;
}

std::string Token::content()
{
    if (m_type == TOKEN_IDENTIFIER || m_type == TOKEN_STRING)
        return m_str;
    return to_string(m_int);
}

std::ostream& operator<<(std::ostream& os, Token const& tok)
{
    std::string type;
    std::string s;

    switch (tok.type())
    {
    case TOKEN_IDENTIFIER:
        type = "Ident";
        break;
    case TOKEN_NUMBER:
        type = "Number";
        break;
    case TOKEN_STRING:
        type = "String";
        break;
    case TOKEN_LEFT_CURLY:
        type = "`{`";
        break;
    case TOKEN_RIGHT_CURLY:
        type = "`}`";
        break;
    case TOKEN_LINE_BREAK:
        type = "`\\n`";
        break;
    case TOKEN_INVALID:
        type = "INVALID";
        break;
    case TOKEN_EOF:
        type = "EOF";
        break;
    }

    os << "Token { type = " << type;

    if (tok.type() == TOKEN_STRING)
        os << ", content = \"" << tok.str() << "\" }";
    else if (tok.type() == TOKEN_IDENTIFIER)
        os << ", content = " << tok.str() << " }";
    else if (tok.type() == TOKEN_NUMBER)
        os << ", content = " << tok.str() << " }";
    else
        os << " }";

    return os;
}

ConfigEntry::ConfigEntry()
{
}

ConfigEntry::~ConfigEntry()
{
}

bool ConfigEntry::is_inline()
{
    return m_inline;
}

void ConfigEntry::set_inline(bool b)
{
    m_inline = b;
}

void ConfigEntry::add_arg(Token tok)
{
    m_arguments.push_back(tok);
}

void ConfigEntry::add_child(ConfigEntry entry)
{
    m_children.push_back(entry);
}

ConfigParser::ConfigParser()
{
}

ConfigParser::~ConfigParser()
{
}

// This will only parse one entry of the file until there is no more entries between `start` and `end`.
// `index` is a cursor which will be updated for each call.
static Option<Result<ConfigEntry, ConfigError> > _parse(std::string& source, std::vector<Token>& tokens, size_t *start,
                                                        size_t *end, size_t *index)
{
    // Two possibles possible entries:
    // 1. An inline entry ended by a line return
    // 2. A group entries which contains a body with curly brackets

    (void)start;

    ConfigEntry entry;
    size_t i = *index;

    for (; i < *end && tokens[i].type() == TOKEN_LINE_BREAK; i++)
    {
    }

    if (i >= *end)
        return None<Result<ConfigEntry, ConfigError> >();

    for (; i < *end && (tokens[i].type() != TOKEN_LINE_BREAK && tokens[i].type() != TOKEN_LEFT_CURLY); i++)
    {
        entry.add_arg(tokens[i]);
    }

    // for (size_t i = 0; i < entry.args().size(); i++)
    //     std::cout << entry.args()[i] << "\n";

    *index = i + 1;
    entry.set_source(source);

    if (i >= *end || tokens[i].type() == TOKEN_LINE_BREAK)
    {
        entry.set_inline(true);
        return Some(Ok<ConfigEntry, ConfigError>(entry));
    }
    else if (tokens[i].type() == TOKEN_LEFT_CURLY && entry.args().size() == 0)
    {
        // A lone `{` without any arguments
        return Some(Err<ConfigEntry, ConfigError>(ConfigError::unexpected(source, tokens[i], TOKEN_IDENTIFIER)));
    }
    else if (tokens[i].type() != TOKEN_LEFT_CURLY)
    {
        return Some(Err<ConfigEntry, ConfigError>(ConfigError::unexpected(source, tokens[i], TOKEN_LEFT_CURLY)));
    }

    Token& left_curly = tokens[i];

    i++;

    size_t new_start = i;

    int open = 0;
    bool end_reached = false;
    for (; i < *end; i++)
    {
        if (tokens[i].type() == TOKEN_RIGHT_CURLY && open == 0)
        {
            end_reached = true;
            break;
        }
        else if (tokens[i].type() == TOKEN_LEFT_CURLY)
            open++;
        else if (tokens[i].type() == TOKEN_RIGHT_CURLY)
            open--;
    }

    if (open != 0 || !end_reached)
        return Err<ConfigEntry, ConfigError>(ConfigError::mismatch_curly(source, left_curly));

    Token& right_curly = tokens[i];
    entry.set_curly(left_curly, right_curly);

    // Not 100% sure of this fix but it seems to work.
    *index = i + 1;

    size_t new_end = i;
    size_t new_index = new_start;
    Option<Result<ConfigEntry, ConfigError> > maybe_entry;

    while ((maybe_entry = _parse(source, tokens, &new_start, &new_end, &new_index)).is_some())
    {
        Result<ConfigEntry, ConfigError> new_entry = maybe_entry.unwrap();
        EXPECT_OK(ConfigEntry, ConfigError, new_entry);
        entry.add_child(new_entry.unwrap());
    }

    return Ok<ConfigEntry, ConfigError>(entry);
}

Result<int, ConfigError> ConfigParser::parse(std::string filename)
{
    struct stat sb;

    if (stat(filename.c_str(), &sb) == -1 || !S_ISREG(sb.st_mode))
        return Err<int, ConfigError>(ConfigError::not_found(filename));
    std::ifstream ifs(filename.data());
    if (!ifs.is_open())
        return Err<int, ConfigError>(ConfigError::not_found(filename));
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    std::vector<Token> tokens = _read_tokens(content);

    // for (size_t i = 0; i < tokens.size(); i++)
    // {
    //     std::cout << tokens[i] << "\n";
    // }

    size_t start = 0;
    size_t end = tokens.size();
    size_t index = start;
    Option<Result<ConfigEntry, ConfigError> > maybe_entry;

    while ((maybe_entry = _parse(content, tokens, &start, &end, &index)).is_some())
    {
        Result<ConfigEntry, ConfigError> entry = maybe_entry.unwrap();
        EXPECT_OK(int, ConfigError, entry);
        m_root.add_child(entry.unwrap());
    }

    return Ok<int, ConfigError>(0);
}

ConfigEntry& ConfigParser::root()
{
    return m_root;
}

static bool _is_space(char c)
{
    return c == ' ' || c == '\t' || c == '\v' || c == '\f'; // || c == '\r' || c == '\n'
}

static bool _is_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r' || c == '\n';
}

static bool _is_all_digits(std::string& s)
{
    for (size_t i = 0; i < s.size(); i++)
        if (s[i] < '0' || s[i] > '9')
            return false;
    return s.size() > 0;
}

static Option<Token> _next_token(size_t *index, std::string source, size_t *line, size_t *column)
{
    std::string buf;
    size_t i = *index;
    bool is_string = false;

    if (i >= source.size())
        return None<Token>();

    while (i < source.size() && _is_space(source[i]))
    {
        i++;
        *column += 1;
    }

    if (source[i] == '\r' || source[i] == '\n')
    {
        *index = i + 1;
        *line += 1;
        *column = 0;
        return Some(Token::ln(*line, *column));
    }

    size_t start_column = *column;

    if (source[i] == '"')
    {
        i++;
        while (i < source.size() && source[i] != '"')
        {
            buf += source[i++];
            *column += 1;
        }
        i++;
        *column += 1;
        is_string = true;
    }
    else if (source[i] == '\'')
    {
        i++;
        while (i < source.size() && source[i] != '\'')
        {
            buf += source[i++];
            *column += 1;
        }
        i++;
        *column += 1;
        is_string = true;
    }
    else
    {
        while (i < source.size() && !_is_whitespace(source[i]))
        {
            buf += source[i++];
            *column += 1;
        }
    }

    *index = i;

    if (buf == "{")
        return Some(Token::left_curly(*line, start_column));
    else if (buf == "}")
        return Some(Token::right_curly(*line, start_column));
    else if (_is_all_digits(buf))
    {
        int64_t i = std::atoll(buf.c_str());
        return Some(Token::num(i, *line, start_column));
    }
    else if (is_string)
        return Some(Token::str(buf, *line, start_column));

    if (!buf.empty())
        return Some(Token::ident(buf, *line, start_column));
    return None<Token>();
}

std::vector<Token> ConfigParser::_read_tokens(std::string source)
{
    size_t index = 0;
    Option<Token> token;
    std::vector<Token> tokens;
    size_t line = 0, column = 0;

    while ((token = _next_token(&index, source, &line, &column)).is_some())
        tokens.push_back(token.unwrap());

    return tokens;
}

/*
    Error handling
 */

ConfigError::ConfigError()
{
}

ConfigError::ConfigError(Type type, Token token, std::string source) : m_source(source), m_token(token), m_type(type)
{
}

ConfigError ConfigError::not_found(std::string filename)
{
    ConfigError err(ConfigError::FILE_NOT_FOUND, Token::invalid(), "");
    err.m_notfound.filename = filename;
    return err;
}

ConfigError ConfigError::unexpected(std::string source, Token got, TokenType expected)
{
    ConfigError err(ConfigError::UNEXPECTED_TOKEN, got, source);
    err.m_unexpected.type = expected;
    return err;
}

ConfigError ConfigError::not_inline(std::string source, Token name)
{
    ConfigError err(ConfigError::NOT_INLINE, name, source);
    return err;
}

ConfigError ConfigError::mismatch_curly(std::string source, Token curly)
{
    ConfigError err(ConfigError::MISMATCH_CURLY, curly, source);
    return err;
}

ConfigError ConfigError::address(std::string source, Token addr)
{
    ConfigError err(ConfigError::ADDR, addr, source);
    return err;
}

ConfigError ConfigError::mismatch_entry(std::string source, Token tok, std::string expected, std::vector<Arg> args)
{
    ConfigError err(ConfigError::MISMATCH_ENTRY, tok, source);
    err.m_mismatch_entry.usage = Usage(expected, args);
    return err;
}

ConfigError ConfigError::unknown_entry(std::string source, Token tok, std::vector<std::string> entries)
{
    ConfigError err(ConfigError::UNKNOWN_ENTRY, tok, source);
    err.m_unknown.entries = entries;
    return err;
}

ConfigError ConfigError::invalid_method(std::string source, Token tok)
{
    ConfigError err(ConfigError::INVALID_METHOD, tok, source);
    return err;
}

ConfigError ConfigError::not_in_range(std::string source, Token tok, int min, int max)
{
    ConfigError err(ConfigError::NOT_IN_RANGE, tok, source);
    err.m_range.min = min;
    err.m_range.max = max;
    return err;
}

std::string ConfigError::_spaces(size_t n)
{
    std::string s;

    if (n == (size_t)-1)
        return "";
    for (size_t i = 0; i < n; i++)
        s += " ";
    return s;
}

std::string ConfigError::_strerror()
{
    switch (m_type)
    {
    case FILE_NOT_FOUND:
        return "Cannot read file `" + m_notfound.filename + "`";
    case UNEXPECTED_TOKEN:
        return "Expected `" + _strtok(m_unexpected.type) + "` but `" + _strtok(m_token.type()) + "` was found";
    case NOT_INLINE:
        return "Expected inline declaration for `" + m_token.content() + "`";
    case MISMATCH_CURLY:
        return "Mismatch curly brackets";
    case MISMATCH_ENTRY: {
        std::string usage;

        usage += m_mismatch_entry.usage.name();

        for (size_t i = 0; i < m_mismatch_entry.usage.args().size(); i++)
            usage += " <" + m_mismatch_entry.usage.args()[i].name() + ">";

        return "Invalid entry, expected usage is `" + usage + "`";
    }
    case UNKNOWN_ENTRY: {
        std::string list;

        if (m_unknown.entries.size() > 0)
            list = "`" + m_unknown.entries[0] + "`";

        for (size_t i = 1; i < m_unknown.entries.size(); i++)
            list += ", `" + m_unknown.entries[i] + "`";

        return "Unknown entry `" + m_token.content() + "` expected one of " + list;
    }
    case ADDR:
        return "Invalid address";
    case INVALID_METHOD:
        return "Invalid method `" + m_token.content() + "` expected one of GET, POST, DELETE";
    case NOT_IN_RANGE:
        return "Value " + m_token.content() + " is not in range " + to_string(m_range.min) + ".." +
               to_string(m_range.max);
    }
}

std::string ConfigError::_strtok(TokenType type)
{
    switch (type)
    {
    case TOKEN_INVALID:
        return "INVALID";
    case TOKEN_IDENTIFIER:
        return "Identifier";
    case TOKEN_STRING:
        return "String";
    case TOKEN_NUMBER:
        return "Number";
    case TOKEN_LEFT_CURLY:
        return "Left curly bracket";
    case TOKEN_RIGHT_CURLY:
        return "Right curly bracket";
    case TOKEN_LINE_BREAK:
        return "Line return";
    case TOKEN_EOF:
        return "End of file";
    }
}
