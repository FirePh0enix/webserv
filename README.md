# Webserv

## Screenshots (it also works on Safari ???)

1. Our beautifully designed file upload page. It features two forms on a clean, minimalist layout :)
<img width="1440" alt="Screenshot 2024-11-03 at 19 06 06" src="https://github.com/user-attachments/assets/b19ab23e-61e7-4295-8bb8-33ea350af322">

2. Directory listing: Firefox front-end developers should hire me! I replicated the directory listing page with the same level of detail (almost) as seen in the Firefox browser.
<img width="1440" alt="Screenshot 2024-11-03 at 19 06 56" src="https://github.com/user-attachments/assets/f1c73de9-4ca8-4303-9b2b-da52c2e99697">

3. Two of our custom error pages (featuring cats, which I think are better than the rest of the error themes).
<img width="1440" alt="Screenshot 2024-11-03 at 19 10 55" src="https://github.com/user-attachments/assets/05cfe889-638a-41a3-b71d-caadf5f2be35">
<img width="1440" alt="Screenshot 2024-11-03 at 19 11 03" src="https://github.com/user-attachments/assets/3c7a6de3-0430-4692-bac9-9d2232a1b3d6">

4. Of course, I won't miss a chance to showcase my portfolio!
<img width="1440" alt="Screenshot 2024-11-03 at 19 09 01" src="https://github.com/user-attachments/assets/22c8b1b9-31b8-47bc-8355-cd18934c6e60">

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

[FirePh0enix](https://github.com/FirePh0enix): An outstanding teammate who was in charge of :
* Designing the architecture of our web server, including the design of most of the classes.
* Parsing the configuration files, ensuring there were no errors.
* Parsing the HTTP requests to populate our internal classes and structures, allowing me to access the necessary variables for my part.
* Managing error handling, including customized error pages, setting up routers, multiple servers, and the logger.
* Managing the MIME types table.
