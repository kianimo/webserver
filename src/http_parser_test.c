#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "http_parser.h"

static size_t numdigits(int n) {
  size_t digits = 0;
  while (n) {
    n /= 10;
    ++digits;
  }
  return digits;
}

static void info(const char *message) { printf("INFO: %s.\n", message); }

static void info_with_data(const char *message, const char *buf, size_t buflen) {
  const size_t messagelen = strlen(message);
  static const char *str1 = /* buflen */ " bytes of data '";
  const size_t str1len = strlen(str1);
  static const char *str2 = /* buf */ "'";
  const size_t str2len = strlen(str2);
  const size_t ndigits = numdigits(buflen);
  const size_t len = messagelen + ndigits + str1len + buflen + str2len + 1;
  char s[len];

  int pos = 0;

  memcpy(s + pos, message, messagelen);
  pos += messagelen;
  snprintf(s + pos, ndigits + 1, "%zu", buflen);
  pos += ndigits;
  memcpy(s + pos, str1, str1len);
  pos += str1len;
  memcpy(s + pos, buf, buflen);
  pos += buflen;
  memcpy(s + pos, str2, str2len);
  pos += str2len;

  s[pos] = 0;

  info(s);
}

static int message_begin_cb(http_parser *p) {
  info("message_begin_cb called");
  return 0;
}

int request_url_cb(http_parser *p, const char *buf, size_t len) {
  info_with_data("request_url_cb called: ", buf, len);
  return 0;
}

int header_field_cb(http_parser *p, const char *buf, size_t len) {
  info_with_data("header_field_cb called: ", buf, len);
  return 0;
}

int header_value_cb(http_parser *p, const char *buf, size_t len) {
  info_with_data("header_value_cb called: ", buf, len);
  return 0;
}

int headers_complete_cb(http_parser *p) {
  info("headers_complete_cb called");
  return 0;
}

int request_body_cb(http_parser *p, const char *buf, size_t len) {
  info_with_data("request_body_cb called: ", buf, len);
  return 0;
}

int message_complete_cb(http_parser *p) {
  info("message_complete_cb called");
  return 0;
}

int do_parser_test(void) {
  // A test HTTP request
  const char *testrequest =
      "GET /favicon.ico HTTP/1.1\r\n"
      "Host: 0.0.0.0=5000\r\n"
      "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) "
      "Gecko/2008061015 Firefox/3.0\r\n"
      "Accept: "
      "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
      "Accept-Language: en-us,en;q=0.5\r\n"
      "Accept-Encoding: gzip,deflate\r\n"
      "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
      "Keep-Alive: 300\r\n"
      "Connection: keep-alive\r\n"
      "\r\n";
  const size_t testrequest_len = strlen(testrequest);

  // HTTP parser config (i.e., the callbacks it should call)
  http_parser_settings settings = { .on_message_begin = message_begin_cb,
                                    .on_header_field = header_field_cb,
                                    .on_header_value = header_value_cb,
                                    .on_url = request_url_cb,
                                    .on_status = 0,
                                    .on_body = request_body_cb,
                                    .on_headers_complete = headers_complete_cb,
                                    .on_message_complete = message_complete_cb };

  // Initialise the HTTP request parser
  http_parser parser;
  http_parser_init(&parser, HTTP_REQUEST);

  // Parse the request
  int nparsed = http_parser_execute(&parser, &settings, testrequest, testrequest_len);
  if (nparsed != testrequest_len) {
    // Fehler koennen erzwungen werden, wenn z.B. statt Methode "GET" "XYZ"
    // verwendet wird.
    enum http_errno err = HTTP_PARSER_ERRNO(&parser);
    fprintf(stderr, "Error while parsing HTTP request:\n\t- Error name: "
                    "'%s'\n\t- Error description: '%s'",
            http_errno_name(err), http_errno_description(err));
    exit(EXIT_FAILURE);
  }
}

