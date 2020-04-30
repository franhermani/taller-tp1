#include "client_parser.h"
#include <stdio.h>
#include <string.h>

#define OK 0
#define ERROR -1

int parser_create(parser_t *self, const char *file_path) {
    dbus_t dbus;
    dbus_create(&dbus);

    dynamic_buffer_t *dyn_buf = dynamic_buffer_create();
    if (! dyn_buf) return ERROR;

    self->file_path = file_path;
    self->dyn_buf = dyn_buf;
    self->dbus = dbus;
    return OK;
}

int parser_parse_input_file(parser_t *self) {
    FILE *fd;
    if (! (fd = fopen(self->file_path, "r"))) {
        printf("Error opening file\n");
        return ERROR;
    }

    int i, s = 1;

    /*
    bool eol;
    while (! feof(fd)) {
        i = 0;
        eol = false;
        while ((s != 0) && (i < sizeof(self->stat_buf))) {
            s = fread(&self->stat_buf[i], sizeof(char), 1, fd);
            if (self->stat_buf[i] == '\n') {
                eol = true;
                break;
            }
            i ++;
        }
        self->stat_buf[i] = '\0';
        if (eol) {
            // TODO: aplicar protocolo dbus a esta linea
            //dynamic_buffer_remove_data(self->dyn_buf);
        } else {
            //dynamic_buffer_insert_data(self->dyn_buf, self->stat_buf);
        }
        memset(&self->stat_buf, 0, sizeof(self->stat_buf));
    }
    //dynamic_buffer_destroy(self->dyn_buf);
    */

    while (! feof(fd)) {
        i = 0;
        while (s != 0) {
            s = fread(&self->stat_buf[i], sizeof(char), 1, fd);
            if (self->stat_buf[i] == '\n') {
                self->stat_buf[i] = '\0';
                dbus_parse_line(&self->dbus, self->stat_buf);
                break;
            }
            i ++;
        }
        memset(&self->stat_buf, 0, sizeof(self->stat_buf));
    }

    fclose(fd);
    return OK;
}

void parser_destroy(parser_t *self) {
    dynamic_buffer_destroy(self->dyn_buf);
}
