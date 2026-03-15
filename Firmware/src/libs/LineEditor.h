#pragma once

#include <string>
#include <vector>

class OutputStream;

class LineEditor
{
public:
    LineEditor(OutputStream *os);
    virtual ~LineEditor();
    void initial_add(char *buf, size_t len);
    bool add(char *buf, size_t len);
    bool add(char c);
    size_t get_line(char *buf, size_t len);
    void set_history(std::vector<std::string>& history, size_t history_max=16);
    void get_history(std::vector<std::string>& history);
private:
    void putch(char c);
    void add_history(const std::string& line);
    void clear_line();
    void putstr(const char* s);

    std::string buf;
    OutputStream *os;
    size_t pos;
    enum STATES {
        NEXT_CHAR,
        ESC_SEQ1,
        ESC_SEQ2,
        ESC_SEQ3,
        RESET
    };
    STATES state;
    std::vector<std::string> history;
    size_t history_max{16};
    size_t hist_index{0};
};
