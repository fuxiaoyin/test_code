#ifndef GENERATE_FEAT_H
#define GENERATE_FEAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
using namespace std;

struct HtkHeader {
    // Number of samples.
    int htk_sample_kind;
    // Sample period.
    int htk_samples_period;
    // Sample size
    short htk_samples_size;
    // Sample kind.
    unsigned short htk_samples_kind;
};

class GenerateFeat {
public:
    GenerateFeat(int feat_dim, int label_dim,
                 int max_sent_num, int max_sent_len,
                 const char *work_directory, bool write_flag);
    ~GenerateFeat();

    int sent_num() {
        return _sent_num;
    }
    int max_sent_num() {
        return _max_sent_num;
    }

    int  add_one_sent(vector< vector<float> > &feat_mat,
                      vector<int> &label_vec, int length);
    void clear();
    void write();
    void read ();
    void write(char *feat_file, char *label_file, char *desc_file);
    void read (char *feat_file, char *label_file, char *desc_file);

private:
    void swap32(char *c) {
        char t0 = c[0];
        char t1 = c[1];
        c[0] = c[3];
        c[1] = c[2];
        c[2] = t1;
        c[3] = t0;
    }
    void swap16(char *c) {
        char t0 = c[0];
        c[0] = c[1];
        c[1] = t0;
    }
    int get_header_size(const char* feat_file, bool is_print);
private:
    int _feat_dim;
    int _label_dim;
    int _max_sent_num;
    int _max_sent_len;

    int _file_id;
    int _sent_num;
    int _start_sent_id;
    bool _has_label;

    FILE *_fp_data_lst;
    char _work_directory[1024];

    vector< vector< vector<float> > > _feat_mat_vec;
    //vector< vector< vector<float> > > _label_mat_vec;
    vector< vector<int> > _label_vec_vec;
    vector<int> _len_vec;
};

#endif
