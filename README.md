# Webserv

## Screenshots

# Table of Contents
1. [Description](#description)
2. [Installation Instructions](#installation-instructions)
3. [Usage Instructions](#usage-instructions)
4. [Key Features](#key-features)
5. [Acknowledgments](#acknowledgments)

## Description

Webserv is a web server project that is part of the 42 curriculum and is implemented in C++. The primary goal of this project is to create a fully functional HTTP server that can handle multiple client requests concurrently. The server is designed to be compliant with HTTP/1.1 standards and supports various features such as serving static files, handling CGI scripts, and managing multiple virtual hosts.

The project focuses on the core concepts of network programming, including socket creation, binding, listening, and accepting connections. It also delves into advanced topics such as asynchronous I/O using epoll, efficient request parsing, and response generation. The challenge lies in creating a robust and efficient server that can handle high loads and provide a reliable service.

This project is an excellent opportunity to deepen your understanding of network programming and enhance your skills in C++. It tests your knowledge of various programming concepts, including data structures, algorithms, memory management, and system calls.

## Installation Instructions

1. **Check Operating System**: This project is designed to run on Linux systems. Ensure you are running a compatible Linux distribution before proceeding with the installation.

2. **Install a C++ compiler**: If you don't already have a C compiler installed, you will need one to build and use this library. You can install the [Clang compiler](https://clang.llvm.org).
   
- On a Mac, you should already have Clang installed as part of Xcode Command Line Tools. You can confirm this by running clang --version in your terminal. If it's not installed, you'll be prompted to install it.

- On a Linux machine, use the package manager for your distribution. For example, on Ubuntu:
```bash
sudo apt install clang
```

## Usage Instructions

1. **Compile the game**: Navigate to the project directory and compile the game using the `make` command:
```bash
make
```

You can also do `make extra` to view my portfolio :).

This will create an executable file named `webserv`.

2. Select a Configuration: You can either choose a configuration file from the `config` folder or create your own. The provided configuration files are modeled after Nginx configurations, offering a familiar structure and options.

3. Run the Executable: To start the web server, execute the following command in your terminal:
```bash
./webserv configs/multihost.conf
```

4. Now you can open the Firefox browser and navigate to the server's address to start using it as a web server.

For example, navigate to `localhost:port/index.html` if it is specified in the root directory, or `localhost:port/another_page` for another page or script.

## Key Features

- Compatible with Firefox browser.
- Compliant with HTTP/1.1 standards.
- Customizable HTTP responses (e.g., cat, pizza, garden, or dog).
- Default error pages.
- Capable of serving fully static websites.
- File upload handling.
- Supports GET, POST, and DELETE methods.
- Stress-tested for 100% availability using Siege.
- Ability to listen on multiple ports.
- Supports CGI scripts in Bash, PHP, and Python.
- Directory listing.
- HTTP redirection.

I was responsible for the following features:

* Handling all `epoll` event loops: setting up the listening loop and modifying the structure to either `EPOLLIN` or `EPOLLOUT` to determine if we are still receiving the request or are ready to send a response.
* Creating, binding, and listening to the socket.
* Designing the directory listing page, mainly inspired by the original in the Firefox browser.
* Implementing file uploads, including parsing multipart/form-data to handle multiple file uploads.

## Acknowledgments

Special thanks to the following individuals and resources for their contributions to this project:

- [FirePh0enix](https://github.com/FirePh0enix): An outstanding teammate who was in charge of :
* Designing the architecture of our web server, including the design of most of the classes.
* Parsing the configuration files, ensuring there were no errors.
* Parsing the HTTP requests to populate our internal classes and structures, allowing me to access the necessary variables for my part.
* Managing error handling, including customized error pages, setting up routers, multiple servers, and the logger.
* Managing the MIME types table.