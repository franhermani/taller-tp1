#ifndef PARSER_H
#define PARSER_H

// Receives a 'line' and parses it into the 'params_array'
// according to dbus protocol
void parse_line(char *line, char **params_array);

#endif // PARSER_H
