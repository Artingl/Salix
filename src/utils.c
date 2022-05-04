#include <utils.h>

void printI(
        const char*fmt, 
        ...)
{
    va_list args;
    va_start(args, fmt);
    printf("\033[94;1;4mINFO:\033[0m ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

void printE(
        const char*fmt, 
        ...)
{
    va_list args;
    va_start(args, fmt);
    printf("\033[31;1;4mERROR:\033[0m ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

void throwE(char*f, int l, int p, const char*fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("\033[31;1;4mERROR (%s %d:%d):\033[0m ", f, l, p);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

InputData*InputFile(char*path)
{
    if(access(path, F_OK) != 0) {
        printE("File with path '%s' wasn't found!", path);
        exit(1);
    }

    InputData*input = (InputData*)malloc(sizeof(InputData));
    FILE*file = fopen(path, "r+");
    input->path = path;
    fseek(file, 0, SEEK_END);
    input->size = ftell(file);
    rewind(file);
    input->buffer = (char*)malloc(input->size*8);
    fread(input->buffer, input->size, 8, file);

    return input;
}

bool _debug(int i)
{
    static int flag = 1;
    if (i == 1)
        flag = !flag;
    return flag;
}

int _isalpha(int a)
{
    if (a == '_') return 1;
    return isalpha(a);
}

int _isdigit(int a)
{
    return isdigit(a);
}

char*charToPtr(char chr)
{
    char*c = (char*)malloc(16);
    c[0] = chr;
    c[1] = '\0';
    return c;
}

char*parseStrInt(int64_t num)
{
    char*r = (char*)calloc(21, sizeof(char));
    sprintf(r, "%llu", num);
    return r;
}

char*concat(char*v0, char*v1, char*v2)
{
    //optimize this shit (better to completely rewrite)
    uint64_t c = 0;
    if (v2 != NULL)
        c = strlen(v2);
    char*buff = (char*)calloc((strlen(v0) + strlen(v1) + c) * 2, sizeof(char));
    c = 0;
    for (int i =0; i < strlen(v0); i++)
        buff[c++] = v0[i];
    for (int i =0; i < strlen(v1); i++)
        buff[c++] = v1[i];
    if (v2 != NULL)
        for (int i =0; i < strlen(v2); i++)
            buff[c++] = v2[i];
    return buff;
}

uint64_t timeStamp()
{
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

char* str_hash(unsigned char *key)
{
    size_t len = strlen(key);
    
    uint32_t hash, i;
    for(hash = i = 0; i < len; ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    char *s_hash = malloc(78);
    sprintf(s_hash, "%d", hash);
    
    return s_hash;
}

char    *make_ptr(char c)
{
    char*cc = malloc(2);
    cc[0] = c; cc[1] = 0;
    return cc;
}

char    *copy_string(char*c)
{
    char*buf = malloc(strlen(c) + 1);
    strcpy(buf, c);
    return buf;
}

char *format_db_string(char*var, char*c)
{
    // addDataSection("%s db \"%s\", 0\n", hash, item->ast->name);
    char*result = concat(var, " db \"", NULL);
    char*add = malloc(strlen(c)*2);
    char*old_a;

    int offset = 0;
    int offset_ = 0;

    // TODO: fix literals parsing bug
    // (at this moment it has some bugs with parsing literals like \\n, \\t, etc.)

    for (int i = 0; i < strlen(c); i++) {
        add[i + offset] = c[i];
        
        if (c[i-1 + offset_] == '\\') {
            char*new_a = malloc(strlen(add));
            memcpy(new_a, add, strlen(add) - (c[i+offset_] != '\\' ? 2 : 0));
            add = new_a;

            switch (c[i+offset_])
            {
                case '\\': {
                    offset_ += 1;
                    // offset += 1;
                } break;

                case 'n':
                case 't':
                case 'r':
                case '0': {
                    old_a = add;
                    if (c[i+offset_] == 'n') add = concat(add, "\", 0x0a, \"", NULL);
                    if (c[i+offset_] == 't') add = concat(add, "\", 0x09, \"", NULL);
                    if (c[i+offset_] == 'r') add = concat(add, "\", 0x0d, \"", NULL);
                    if (c[i+offset_] == '0') add = concat(add, "\", 0x00, \"", NULL);
                    free((void*)old_a);
                    offset += 8;
                } break;

                default: {
                    printE("Undefined literal '\\%c'!", c[i+offset_]);
                    exit(1);
                }
            }
        }
    }

    char*old_r = result;
    result = concat(result, add, "\", 0\n");
    free((void*)old_r);

    return result;
}

char *format_string(char*c)
{
    return str_replace(
                str_replace(
                        str_replace(
                                str_replace(c, "\\n", "\n"), 
                        "\\0", "\0"), 
                "\\t", "\t"), 
            "\\r", "\r");
}

char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; tmp = strstr(ins, rep); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

void remove_comments(InputData*data) {
    char*prgm = data->buffer;

    int p = 0;
    int n = strlen(prgm);
    char*res = malloc(n*2);
 
    // Flags to indicate that single line and multiple line comments
    // have started or not.
    bool s_cmt = false;
    bool m_cmt = false;
 
 
    // Traverse the given program
    for (int i=0; i<n; i++)
    {
        // If single line comment flag is on, then check for end of it
        if (s_cmt == true && prgm[i] == '\n')
            s_cmt = false;
 
        // If multiple line comment is on, then check for end of it
        else if  (m_cmt == true && prgm[i] == '*' && prgm[i+1] == '/')
            m_cmt = false,  i++;
 
        // If this character is in a comment, ignore it
        else if (s_cmt || m_cmt)
            continue;
 
        // Check for beginning of comments and set the appropriate flags
        else if (prgm[i] == '/' && prgm[i+1] == '/')
            s_cmt = true, i++;
        else if (prgm[i] == '/' && prgm[i+1] == '*')
            m_cmt = true,  i++;
 
        // If current character is a non-comment character, append it to res
        else  res[p++] = prgm[i];
    }

    // res[p++] = '\0';

    data->buffer = res;
    data->size = p;

    free((void*)prgm);
}
