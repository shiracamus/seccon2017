/**
* @file brainhack.c
* @brief Brainf*ck compatible interpriter
* @author @shiracamus
* @date 2018.02.16
* @details
*       Brainf*ck commands:
*               > : increment data pointer, wrap around in data area
*               < : decrement data pointer, wrap around in data area
*               + : increment data
*               - : decrement data
*               . : print data
*               , : input data from keybaord
*               [ : loop begin, if data is 0 then goto the next of loop end
*               ] : loop end, if data is not 0 then goto the next of loop begin
*       changed commands:
*               , : input data from code
*       added commands:
*               { : decrement data pointer, no wrap around (vulnerable)
*               } : increment data pointer, no wrap around (vulnerable)
*               : : print data twice
*               ? : print data in hexadecimal
*               ; : input data from code and print it
*               ' : input data from file
*               " : input data from memory
*               % : swap data pointer and memory pointer
*
*       code area size: 8192 bytes (8KiB)
*       data area size: 32768 bytes (32KiB)
*       initialize data: all 0
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>

#define KiB * 1024
#define CODE_SIZE (8 KiB)
#define DATA_SIZE (32 KiB)
#define DATA_LAST (DATA_SIZE - 1)
#define FLAG_FILENAME "/YXR0YWNrNC50eHQ="  // "/" + base64 of "attack4.txt"
#define FLAG_SIZE (256)
#define INPUT_FILENAME "/attack3.txt"

typedef unsigned char Flag;
typedef unsigned char Code;
typedef unsigned char Data;
typedef unsigned char Memory;
typedef long ProgramCounter;
typedef Data *DataPointer;
typedef Memory *MemoryPointer;
typedef unsigned long CodeSize;
typedef const char _String[];
typedef const char *String;
typedef String Error;

_String ERROR_CODE_TOO_BIG  = "Too large code";
_String ERROR_LOOP_NO_END   = "Not found the end of loop";
_String ERROR_LOOP_NO_BEGIN = "Not found the beginning of loop";

#define ENCODE(data) ((isprint(data) || isspace(data)) ? (data) : '?')

typedef struct _Brainhack {
    ProgramCounter jump[CODE_SIZE];   // This may be used for exploiting
    enum {
        STATE_HALT = 0,
        STATE_RUN,
    } state;
    Error          error;
    CodeSize       code_size;
    ProgramCounter pc;
    DataPointer    dp;  // This is vulnerable, you can read/write anywhere
    MemoryPointer  mp;  // This is vulnerable, you can read/write anywhere
    Flag           flag[FLAG_SIZE];
    Data           data[DATA_SIZE];
    Code           code[CODE_SIZE];
    Code           overflow_guard[2];  // for the code ends with ',' or ';'
} *Brainhack;

static inline Brainhack Brainhack_new(void) {
    Brainhack this = calloc(1, sizeof(*this));
    if (this == NULL) {
        perror("calloc");
        exit(1);
    }
    this->dp = this->data;
    this->mp = this->data;
    return this;
}

static inline Code Brainhack_code(Brainhack this) {
    return this->code[this->pc++];
}

static void Brainhack_error(Brainhack this, String error) {
    this->state = STATE_HALT;
    if (this->error)
        return;
    this->error = error;
    fprintf(stderr, "\nERROR: %s at the code offset %ld\n", error, this->pc);
}

static inline void Brainhack_print(Brainhack this) {
    Data data = *this->dp;
    putchar(ENCODE(data));
    fflush(stdout);
}

static inline void Brainhack_print_twice(Brainhack this) {
    Brainhack_print(this);
    Brainhack_print(this);
}

static inline void Brainhack_print_hex(Brainhack this) {
    Data data = *this->dp;
    putchar("0123456789abcdef"[(data >> 4) & 0xf]);
    putchar("0123456789abcdef"[(data >> 0) & 0xf]);
    fflush(stdout);
}

static inline void Brainhack_next(Brainhack this) {
    if (this->dp >= this->data + DATA_LAST) {
        this->dp = this->data;
    } else {
        this->dp++;
    }
}

static inline void Brainhack_prev(Brainhack this) {
    if (this->dp <= this->data) {
        this->dp = this->data + DATA_LAST;
    } else {
        this->dp--;
    }
}

static inline void Brainhack_prev_vulnerable(Brainhack this) {
    this->dp--;
}

static inline void Brainhack_next_vulnerable(Brainhack this) {
    this->dp++;
}

static inline void Brainhack_inc(Brainhack this) {
    (*this->dp)++;
}

static inline void Brainhack_dec(Brainhack this) {
    (*this->dp)--;
}

static inline void Brainhack_input_from_code(Brainhack this) {
    *this->dp = Brainhack_code(this);
}

static inline void Brainhack_input_from_code_and_print(Brainhack this) {
    Brainhack_input_from_code(this);
    Brainhack_print(this);
}

static inline void Brainhack_input_from_file(Brainhack this) {
    int fd = open(INPUT_FILENAME, O_RDONLY);
    if (fd == -1) return;
    if (lseek(fd, this->dp - this->data, SEEK_SET) == -1
    ||  read(fd, this->dp, sizeof(*this->dp)) != 1) {
        *this->dp = '\0';
    }
    close(fd);
}

static inline void Brainhack_input_from_memory(Brainhack this) {
    *this->dp = *this->mp;
}

static inline void Brainhack_swap_pointer(Brainhack this) {
    DataPointer dp = this->dp;
    this->dp = this->mp;
    this->mp = dp;
}

static inline void Brainhack_begin(Brainhack this) {
    if (*this->dp != 0)
        return;
    ProgramCounter pc = this->pc;
    ProgramCounter jump = this->jump[pc];
    if (jump) {
        this->pc = jump;
        return;
    }
    ProgramCounter origin = pc;
    for (int nest = 1; nest > 0; ) {
        switch (this->code[pc++]) {
        case '[': nest++; break;
        case ']': nest--; break;
        case '\0':
            pc--;
            Brainhack_error(this, ERROR_LOOP_NO_END);
            return;
        }
    }
    this->jump[origin] = pc;
    this->pc = pc;
}

static inline void Brainhack_end(Brainhack this) {
    if (*this->dp == 0)
        return;
    ProgramCounter pc = this->pc;
    ProgramCounter jump = this->jump[pc];
    if (jump) {
        this->pc = jump;
        return;
    }
    ProgramCounter origin = pc;
    for (int nest = 1; nest > 0; ) {
        if (--pc < 1) {
            Brainhack_error(this, ERROR_LOOP_NO_BEGIN);
            return;
        }
        switch (this->code[pc - 1]) {
        case ']': nest++; break;
        case '[': nest--; break;
        }
    }
    this->jump[origin] = pc;
    this->pc = pc;
}

static inline void Brainhack_halt(Brainhack this) {
    this->state = STATE_HALT;
}

static inline void Brainhack_execute(Brainhack this, Code code) {
    switch (code) {
    case '<' : Brainhack_prev(this); return;
    case '>' : Brainhack_next(this); return;
    case '{' : Brainhack_prev_vulnerable(this); return;
    case '}' : Brainhack_next_vulnerable(this); return;
    case '+' : Brainhack_inc(this); return;
    case '-' : Brainhack_dec(this); return;
    case '.' : Brainhack_print(this); return;
    case ':' : Brainhack_print_twice(this); return;
    case '?' : Brainhack_print_hex(this); return;
    case ',' : Brainhack_input_from_code(this); return;
    case ';' : Brainhack_input_from_code_and_print(this); return;
    case '\'': Brainhack_input_from_file(this); return;
    case '"' : Brainhack_input_from_memory(this); return;
    case '%' : Brainhack_swap_pointer(this); return;
    case '[' : Brainhack_begin(this); return;
    case ']' : Brainhack_end(this); return;
    case '\0': this->pc--; Brainhack_halt(this); return;
    default:   /* ignore */ return;
    }
}

static inline void Brainhack_run(Brainhack this) {
    if (this->error)
        return;
    this->state = STATE_RUN;
    while (this->state == STATE_RUN) {
        Brainhack_execute(this, Brainhack_code(this));
    }
}

static inline void Brainhack_load_code(Brainhack this, FILE *input) {
    size_t size = fread(this->code, 1, CODE_SIZE, input);
    if (size >= CODE_SIZE) {
        Brainhack_error(this, ERROR_CODE_TOO_BIG);
        return;
    }
    if (size < 0)
        size = 0;
    this->code[size] = '\0';
    this->code_size = size;
}

static inline void Brainhack_load_flag(Brainhack this) {
    int fd = open(FLAG_FILENAME, O_RDONLY);
    if (fd == -1) return;
    size_t size = read(fd, this->flag, sizeof(this->flag)) + 1;
    for ( ; size < sizeof(this->flag); size++) {
        this->flag[size] = ' ';
    }
    close(fd);
}

static inline void Brainhack_destroy(Brainhack this) {
    free(this);
}

static void stop(int sig) {
    puts("\nERROR: Abort");
    fflush(stdout);
    exit(1);
}

static void timeout(int sig) {
    puts("\nERROR: Timeout");
    fflush(stdout);
    exit(1);
}

int main(int argc, char *argv[])
{
    signal(SIGSEGV, stop);
    signal(SIGBUS, stop);
    signal(SIGILL, stop);
    signal(SIGFPE, stop);
    signal(SIGSYS, stop);
    signal(SIGPIPE, stop);
    signal(SIGALRM, timeout);
    alarm(2);

    Brainhack bh = Brainhack_new();
    Brainhack_load_flag(bh);
    Brainhack_load_code(bh, stdin);
    Brainhack_run(bh);
    Brainhack_destroy(bh);

    printf("\n");
    fflush(stdout);

    return bh->error ? 1 : 0;
}
