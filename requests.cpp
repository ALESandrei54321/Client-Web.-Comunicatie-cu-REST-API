#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "json.hpp"
#include <string>
#include <bits/stdc++.h>
#include <iostream>
#include <string>


char *compute_get_request(char *host, char *url, char *query_params,
                            string cookies[MAX_COOKIES], int cookies_count)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
  if (cookies != NULL) {
    for (int i = 0; i < cookies_count; i++) {
      compute_message(message, std::string(cookies[i]).c_str());
    }
  }
    // Step 4: add final new line
    compute_message(message, "");

    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, nlohmann::json body_data,
                            int body_data_fields_count, string cookies[MAX_COOKIES], int cookies_count)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));
   
    int content_len = 0;
    string body_data_str = body_data.dump();
    content_len = body_data_str.length();

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %d", content_len);
    compute_message(message, line);

   if (cookies != NULL) {
    for (int i = 0; i < cookies_count; i++) {
      compute_message(message, std::string(cookies[i]).c_str());
    }
  }

  compute_message(message, "");

  memset(line, 0, LINELEN);
  if (body_data_fields_count > 0) {
    compute_message(message, body_data_str.c_str());
  }
  
    free(line);
    return message;
}
char *compute_delete_request(char *host, char *url, char *query_params,
							 std::string cookies[MAX_COOKIES], int cookies_count){

    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
  if (cookies != NULL) {
    for (int i = 0; i < cookies_count; i++) {
      compute_message(message, std::string(cookies[i]).c_str());
    }
  }
    // Step 4: add final new line
    compute_message(message, "");

    free(line);
    return message;
}