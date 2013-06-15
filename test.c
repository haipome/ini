# include <stdio.h>
# include <string.h>
# include <error.h>
# include <errno.h>
# include <inttypes.h>
# include <arpa/inet.h>
# include <stdlib.h>
# include <mcheck.h>

# include "ini.h"

int main()
{
    /* $ export MALLOC_TRACE=malloc.log */
    mtrace();

    ini_t *conf = ini_load("test.ini");
    if (conf == NULL)
        error(1, errno, "ini_load fail");

    char *type;
    ini_read_str(conf, "main", "type", &type, "test");
    puts(type);
    free(type);

    char value[100] = { 0 };
    ini_read_strn(conf, "main", "len", value, sizeof(value), NULL);
    puts(value);

    int i;
    ini_read_int(conf, "int", "int", &i, 0);
    unsigned ui;
    ini_read_unsigned(conf, "int", "unsigned", &ui, 0);
    printf("int: %d, unsigned: %u\n", i, ui);

    int8_t  int8;
    ini_read_int8(conf, "int", "int8", &int8, 0);
    uint8_t uint8;
    ini_read_uint8(conf, "int", "uint8", &uint8, 0);
    printf("int8: %i, uint8: %u\n", int8, uint8);

    int16_t int16;
    ini_read_int16(conf, "int", "int16", &int16, 0);
    uint16_t uint16;
    ini_read_uint16(conf, "int", "uint16", &uint16, 0);
    printf("int16: %i, uint16: %u\n", int16, uint16);

    int32_t int32;
    ini_read_int32(conf, "int", "int32", &int32, 0);
    uint32_t uint32;
    ini_read_uint32(conf, "int", "uint32", &uint32, 0);
    printf("int32: %i, uint32: %u\n", int32, uint32);

    int64_t int64;
    ini_read_int64(conf, "int", "int64", &int64, 0);
    uint64_t uint64;
    ini_read_uint64(conf, "int", "uint64", &uint64, 0);
    printf("int64: %"PRIi64", uint64: %"PRIu64"\n", int64, uint64);

    float f;
    ini_read_float(conf, "float", "float", &f, 0);
    double d;
    ini_read_double(conf, "float", "double", &d, 0);
    printf("float: %f, double: %f\n", f, d);

    struct sockaddr_in addr;
    ini_read_ipv4_addr(conf, "addr", "ipv4", &addr, "127.0.0.1:0");
    printf("%s:%u\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    char *proc_name = NULL;
    ini_read_str(conf, "global", "proc_name", &proc_name, "test.ini");
    printf("proc_name: %s\n", proc_name);
    free(proc_name);

    char *phone_num = NULL;
    ini_read_str(conf, "damon", "phone num", &phone_num, "123456789");
    printf("phone num: %s\n", phone_num);
    free(phone_num);

    char *city = NULL;
    ini_read_str(conf, "global", "$%^#@!", &city, "shen zhen");
    puts(city);
    free(city);

    char *name = NULL;
    ini_read_str(conf, NULL, "名字", &name, "");
    puts(name);
    free(name);

    ini_free(conf);

    return 0;
}

