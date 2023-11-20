#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <iostream>
#include <map>
#include <string>
#include "helpers.h"
#include "requests.h"
#include "json.hpp"

#define COMMAND_DIM 100
#define HOST (char*)"34.254.242.81"
#define PORT 8080
#define PAYLOAD_TYPE (char*)"application/json"
#define REGISTER_URL (char*)"/api/v1/tema/auth/register"
#define LOGIN_URL (char*)"/api/v1/tema/auth/login"
#define ACCESS_URL (char*)"/api/v1/tema/library/access"
#define BOOKS_URL (char*)"/api/v1/tema/library/books"
#define LOGOUT_URL (char*) "/api/v1/tema/auth/logout"


using json = nlohmann::json;
using namespace std;


bool containsSubstring(const std::string& text, const std::string& substring) {
    return text.find_first_of(substring) != std::string::npos;
}

bool containsWhitespace(const std::string& str) {
    return str.find_first_of(" \t\n\v\f\r") != std::string::npos;
}
// username: anewuser
// password: pass


std::string extract_cookies(const std::string& text) {
    std::string cookieField;
    size_t start = text.find("Set-Cookie: ");
    if (start != std::string::npos) {
        start += 12;  // Length of "Set-Cookie:" string
        size_t end = text.find("\r\n", start);
        if (end != std::string::npos) {
            cookieField = text.substr(start, end - start);
        }
    }
    return cookieField;
}

std::string extractToken(const std::string& response) {
    std::string token;
    size_t start = response.find("\"token\":\"");
    if (start != std::string::npos) {
        start += 9;  // Length of "\"token\":\"" string
        size_t end = response.find("\"", start);
        if (end != std::string::npos) {
            token = response.substr(start, end - start);
        }
    }
    return token;
}


bool isNumber(const std::string& str) {
    try {
        size_t pos = 0;
        std::stoi(str, &pos);
        return pos == str.length();
    } catch (const std::exception& e) {
        return false;
    }
}


void printBooks(const std::string& httpAnswer) {
    // Extract the JSON data portion from the HTTP answer
    std::size_t jsonStartPos = httpAnswer.find("[{");
    std::size_t jsonEndPos = httpAnswer.rfind("}]");
    if (jsonStartPos == std::string::npos || jsonEndPos == std::string::npos) {
        std::cout << "There are no books here." << std::endl;
        return;
    }

    std::string jsonData = httpAnswer.substr(jsonStartPos, jsonEndPos - jsonStartPos + 2);

    // Parse and print the book data from the JSON
    std::vector<std::string> books;
    std::size_t bookStartPos = jsonData.find("{\"id\":");
    while (bookStartPos != std::string::npos) {
        std::size_t bookEndPos = jsonData.find("}", bookStartPos);
        if (bookEndPos == std::string::npos)
            break;

        std::string bookData = jsonData.substr(bookStartPos, bookEndPos - bookStartPos + 1);

        // Extract the book title and ID from the book data
        std::size_t titleStartPos = bookData.find("\"title\":\"") + 9;
        std::size_t titleEndPos = bookData.find("\"", titleStartPos);
        std::string title = bookData.substr(titleStartPos, titleEndPos - titleStartPos);

        std::size_t idStartPos = bookData.find("\"id\":") + 5;
        std::size_t idEndPos = bookData.find(",", idStartPos);
        int id = std::stoi(bookData.substr(idStartPos, idEndPos - idStartPos));

        books.push_back("\"" + title + "\", id = " + std::to_string(id));

        bookStartPos = jsonData.find("{\"id\":", bookEndPos);
    }

    // Print the formatted book data
    std::cout << "There are " << books.size() << " books in the library:" << std::endl;
    for (const auto& book : books) {
        std::cout << "- " << book << std::endl;
    }
}

void printBookData(const std::string& httpResponse) {
    // Extract the JSON data portion from the HTTP response
    std::size_t jsonStartPos = httpResponse.find("{");
    std::size_t jsonEndPos = httpResponse.rfind("}");
    if (jsonStartPos == std::string::npos || jsonEndPos == std::string::npos) {
        std::cout << "There are no books in the library." << std::endl;
        return;
    }

    std::string jsonData = httpResponse.substr(jsonStartPos, jsonEndPos - jsonStartPos + 1);

    // Parse the JSON data
    nlohmann::json bookData;
    try {
        bookData = nlohmann::json::parse(jsonData);
    } catch (const nlohmann::json::exception& e) {
        std::cout << "Failed to parse JSON data: " << e.what() << std::endl;
        return;
    }

    // Print the book data
    std::cout << "- Title: " << bookData["title"] << std::endl;
    std::cout << "- Author: " << bookData["author"] << std::endl;
    std::cout << "- Publisher: " << bookData["publisher"] << std::endl;
    std::cout << "- Genre: " << bookData["genre"] << std::endl;
    std::cout << "- Page_count: " << bookData["page_count"] << std::endl;
}


int main() {

    int sockfd;
    char *message;
    char *response;
    string login_cookie;
    string token = "";

    std::cout << "Welcome to the library authentification menu!" << endl;

    while (1) {

        char* line = (char*) malloc (COMMAND_DIM * sizeof(char));
        fgets(line, COMMAND_DIM, stdin);

        if (strcmp(line, "exit\n") == 0) {

            std::cout << "Exiting app..." << endl;
            break;

        } else if (strcmp(line, "register\n") == 0) {

            string user, pass;
            std::cout << "username=";
            std::getline(std::cin, user);

            std::cout << "password=";
            std::getline(std::cin, pass);

            if (user.empty()) {
                std::cout << "Error! Empty string detected." << endl;
                continue;
            }

            if (containsWhitespace(user)) {
                std::cout << "Error! Whitespace detected." << endl;
                continue;
            }

            if (pass.empty()) {
                std::cout << "Error! Empty string detected." << endl;
                continue;
            }

            if (containsWhitespace(pass)) {
                std::cout << "Error! Whitespace detected." << endl;
                continue;
            }

            json data;
            
            data["password"] = pass;
            data["username"] = user;

            sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);

            message = compute_post_request(HOST, REGISTER_URL, PAYLOAD_TYPE, data, 1, NULL, 0);

            send_to_server(sockfd, message);
            
            response = receive_from_server(sockfd);

            close_connection(sockfd);

            if (strstr(response, "error") != NULL) {
                std::cout << "An error has occured during register: ";
                std::cout << "Username already taken." << endl;
                continue;
            } else {
                std::cout << "200 - OK - Utilizator înregistrat cu succes!" << endl;
            }

        } else if (strcmp(line, "login\n") == 0) {
            
            string user, pass;
            std::cout << "username=";
            std::getline(std::cin, user);

            std::cout << "password=";
            std::getline(std::cin, pass);

            if (user.empty()) {
                std::cout << "Error! Empty string detected." << endl;
                continue;
            }

            if (containsWhitespace(user)) {
                std::cout << "Error! Whitespace detected." << endl;
                continue;
            }

            if (pass.empty()) {
                std::cout << "Error! Empty string detected." << endl;
                continue;
            }

            if (containsWhitespace(pass)) {
                std::cout << "Error! Whitespace detected." << endl;
                continue;
            }

            json data;
            data["username"] = user;
            data["password"] = pass;
            
            sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);
            
            message = compute_post_request(HOST, LOGIN_URL, PAYLOAD_TYPE, data, 1, NULL, 0);
            
            //std :: cout << message << endl;

            send_to_server(sockfd, message);
            
            response = receive_from_server(sockfd);

            //std::cout << response;

            close_connection(sockfd);

            if (strstr(response, "error") != NULL) {
                std::cout << "An error has occured during login: ";

                if (strstr(response, "No account with this username!") == NULL) {
                    std::cout << "Username or passward is incorrect." << endl;
                    continue;
                } else {
                    std::cout << "No account with this username has been found." << endl;
                    continue;
                }

                
            } else {
                std::cout << "200 - OK - Bun venit!" << endl;
                login_cookie = extract_cookies(response);
                //std :: cout << login_cookie << endl;
            }

            

        } else if (strcmp(line, "enter_library\n") == 0) {
            if (!containsSubstring(login_cookie, "connect.sid")) {
                std::cout << "An error has occured during enter_library: ";
                std::cout << "You are not logged in." << endl;
                continue;
            } else {
                std :: cout << "Welcome to the library!" << endl;
            }

            string actual_cookie_line = "Cookie: ";
            actual_cookie_line.append(login_cookie);

            string cookie_array[1];
            cookie_array[0] = actual_cookie_line;

            sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);
            
            message = compute_get_request(HOST, ACCESS_URL, NULL, cookie_array, 1);
            
            //std :: cout << message << endl;

            send_to_server(sockfd, message);
            
            response = receive_from_server(sockfd);

            //std::cout << response;

            close_connection(sockfd);

            token = extractToken(response);


        } else if (strcmp(line, "get_books\n") == 0) {
            
            if (token == "") {
                std::cout << "An error has occured during get_books: ";
                std::cout << "No access to library. You need to login and enter the library first." << endl;
                continue; 
            }

            string tok_array[1];
            tok_array[0] = "Authorization: Bearer " + token;

            sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);

            message = compute_get_request(HOST, BOOKS_URL, NULL, tok_array, 1);

            //std :: cout << message << endl;

            send_to_server(sockfd, message);
            
            response = receive_from_server(sockfd);

            //std::cout << response;

            close_connection(sockfd);

            printBooks(response);

        } else if (strcmp(line, "get_book\n") == 0) {
            if (token == "") {
                std::cout << "An error has occured during get_book: ";
                std::cout << "No access to library. You need to login and enter the library first." << endl;
                continue; 
            }

            string tok_array[1];
            tok_array[0] = "Authorization: Bearer " + token;

            string id;
            std::cout << "id=";
            std::getline(std::cin,id);

            if (id.empty()) {
                std::cout << "An error has occured during get_book: ";
                std::cout << "You must complete all fields." << endl;
                continue;
            }

            if (!isNumber(id)) {
                std::cout << "An error has occured during get_book: ";
                std::cout << "The field \"id\" is not a valid number." << endl;
                continue;
            }

            sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);


            // Concatenate the BOOKS_URL, '/', and id into a new string
            std::string bookUrlString = std::string(BOOKS_URL) + '/' + id;

            // Convert the string to a char*
            char* bookUrl = new char[bookUrlString.length() + 1];
            std::strcpy(bookUrl, bookUrlString.c_str());

            message = compute_get_request(HOST, bookUrl, NULL, tok_array, 1);

            delete bookUrl;

            //std :: cout << message << endl;

            send_to_server(sockfd, message);
            
            response = receive_from_server(sockfd);

            //std::cout << response;

            close_connection(sockfd);

            if (strstr(response, "200 OK") != NULL) {
                printBookData(response);
            } else {
                std::cout << "An error has occured during get_book: ";
                std::cout << "There was an error." << endl;
            }



        } else if (strcmp(line, "add_book\n") == 0) {
            if (token == "") {
                std::cout << "An error has occured during add_book: ";
                std::cout << "No access to library. You need to login and enter the library first." << endl;
                continue; 
            }

            string tok_array[1];
            tok_array[0] = "Authorization: Bearer " + token;

            string title, auth, gen, pc, pub;

            std::cout << "title=";
            std::getline(std::cin,title);
            std::cout << "author=";
            std::getline(std::cin,auth);
            std::cout << "genre=";
            std::getline(std::cin,gen);
            std::cout << "publisher=";
            std::getline(std::cin,pub);
            std::cout << "page_count=";
            std::getline(std::cin,pc);

            if (title.empty() || auth.empty() || gen.empty() || pub.empty() || pc.empty()) {
                std::cout << "An error has occured during add_book: ";
                std::cout << "You must complete all fields." << endl;
                continue;
            }

            if (!isNumber(pc)) {
                std::cout << "An error has occured during add_book: ";
                std::cout << "The field \"page_count\" is not a valid number." << endl;
                continue;
            }

            json data;
            data["title"] = title;
            data["author"] = auth;
            data["genre"] = gen;
            data["publisher"] = pub;
            data["page_count"] = pc;

            sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);

            message = compute_post_request(HOST, BOOKS_URL, PAYLOAD_TYPE, data, 1, tok_array, 1);

            //std :: cout << message << endl;

            send_to_server(sockfd, message);
            
            response = receive_from_server(sockfd);

            //std::cout << response;

            close_connection(sockfd);
            
            if (strstr(response, "200 OK") != 0) {
                std::cout << "Book was added succesfully." << endl;
            } else {
                std::cout << "An error has occured during add_book: ";
                std::cout << "The book was not added." << endl;
            }

        } else if (strcmp(line, "delete_book\n") == 0) {
            if (token == "") {
                std::cout << "An error has occured during delete_book: ";
                std::cout << "No access to library. You need to login and enter the library first." << endl;
                continue; 
            }

            string tok_array[1];
            tok_array[0] = "Authorization: Bearer " + token;

            string id;
            std::cout << "id=";
            std::getline(std::cin,id);

            if (id.empty()) {
                std::cout << "An error has occured during delete_book: ";
                std::cout << "You must complete all fields." << endl;
                continue;
            }

            if (!isNumber(id)) {
                std::cout << "An error has occured during delete_book: ";
                std::cout << "The field \"id\" is not a valid number." << endl;
                continue;
            }

            sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);

            // Concatenate the BOOKS_URL, '/', and id into a new string
            std::string bookUrlString = std::string(BOOKS_URL) + '/' + id;

            // Convert the string to a char*
            char* bookUrl = new char[bookUrlString.length() + 1];
            std::strcpy(bookUrl, bookUrlString.c_str());

            message = compute_delete_request(HOST, bookUrl, NULL, tok_array, 1);

            delete bookUrl;

            //std :: cout << message << endl;

            send_to_server(sockfd, message);
            
            response = receive_from_server(sockfd);

            //std::cout << response;

            close_connection(sockfd);

            if (strstr(response, "200 OK") != NULL) {
                std::cout << "Book deleted successfully." << endl;
            } else {
                std::cout << "An error has occured during get_book: ";
                std::cout << "There was an error." << endl;
            }




        } else if (strcmp(line, "logout\n") == 0) {
            //std::cout << "Logout?" << endl;
            if (!containsSubstring(login_cookie, "connect.sid")) {
                std::cout << "An error has occured during logout: ";
                std::cout << "You are not logged in." << endl;
                continue;
            }

            string actual_cookie_line = "Cookie: ";
            actual_cookie_line.append(login_cookie);

            string cookie_array[1];
            cookie_array[0] = actual_cookie_line;

            sockfd = open_connection(HOST, PORT, PF_INET, SOCK_STREAM, 0);
            
            message = compute_get_request(HOST, LOGOUT_URL, NULL, cookie_array, 1);
            
            //std :: cout << message << endl;

            send_to_server(sockfd, message);
            
            response = receive_from_server(sockfd);

            //std::cout << response;

            close_connection(sockfd);

            if (strstr(response, "200 OK") == 0) {
                std::cout << "An error has occured during logout: ";
                std::cout << "Something went wrong." << endl;
                continue;
            }

            login_cookie = "";
            token = "";

        }

    }


}