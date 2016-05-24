#include "generate_feat.h"

GenerateFeat::~GenerateFeat()
{
}

void GenerateFeat::clear()
{
    _sent_num = 0;
}

int GenerateFeat::add_one_sent(vector< vector<float> > &feat_mat,
                               vector<int> &label_vec, int length)
{
    if (length > _max_sent_len) {
        printf("the sentence is too long!");
        return 1;
    }
    for (int ii = 0; ii < length; ii++) {
        //memcpy(&_feat_mat_vec[_sent_num][ii][0], &feat_mat[ii][0],
        //        sizeof(float) * _feat_dim);
        _feat_mat_vec[_sent_num][ii] = feat_mat[ii];
        if (_has_label) {
            _label_vec_vec[_sent_num][ii] = label_vec[ii];
        }
    }
    _len_vec[_sent_num] = length;
    //
    _sent_num++;
    return 0;
}

void GenerateFeat::write()
{
    char feat_file[1024], label_file[1024], desc_file[1024];
    sprintf(feat_file, "%s/%d-%d.feat", _work_directory,
            _start_sent_id, _start_sent_id + _sent_num - 1);
    sprintf(label_file, "%s/%d-%d.label", _work_directory,
            _start_sent_id, _start_sent_id + _sent_num - 1);
    sprintf(desc_file, "%s/%d-%d.desc", _work_directory,
            _start_sent_id, _start_sent_id + _sent_num - 1);
    _start_sent_id += _sent_num;

    if (_has_label) {
        fprintf(_fp_data_lst, "%s\n%s\n%s\n", feat_file,
                label_file, desc_file);
    } else {
        fprintf(_fp_data_lst, "%s\n%s\n", feat_file, desc_file);
    }
    //
    write(feat_file, label_file, desc_file);
    clear();
}

void GenerateFeat::write(char *feat_file, char *label_file, char *desc_file)
{
    FILE *fp_feat  = NULL;
    FILE *fp_label = NULL;
    FILE *fp_desc  = NULL;
    fp_feat = fopen(feat_file, "wb");
    if (fp_feat == NULL) {
        printf("Cannot open file %s to write!\n", feat_file);
    }
    if (_has_label) {
        fp_label = fopen(label_file, "wb");
        if (fp_label == NULL) {
            printf("Cannot open file %s to write!\n", label_file);
        }
    }
    fp_desc = fopen(desc_file, "wt");
    if (fp_desc == NULL) {
        printf("Cannot open file %s to write!\n", desc_file);
    }

    int pre_len       = 3;
    int sample_num    = pre_len;
    int sample_period = 50000;
    short sample_size = _feat_dim * sizeof(float);
    short sample_kind = 9;
    for (int ii = 0; ii < _sent_num; ii++) {
        sample_num += _len_vec[ii];
    }
    // the htk head
    swap32((char*)&sample_num);
    swap32((char*)&sample_period);
    swap16((char*)&sample_size);
    swap16((char*)&sample_kind);
    fwrite(&sample_num,    sizeof(int), 1, fp_feat);
    fwrite(&sample_period, sizeof(int), 1, fp_feat);
    fwrite(&sample_size,   sizeof(short), 1, fp_feat);
    fwrite(&sample_kind,   sizeof(short), 1, fp_feat);

    int sent_id   = 0;
    int start_pos = 0;
    // the reverse info
    vector<float> pre_feat_vec;
    pre_feat_vec.resize(_feat_dim, 0.0f);
    int pre_label = 0;
    for (int ii = 0; ii < pre_len; ii++) {
        fwrite(&pre_feat_vec[0], sizeof(float), _feat_dim, fp_feat);
        if (_has_label) {
            fwrite(&pre_label, sizeof(int), 1, fp_label);
        }
    }
    fprintf(fp_desc, "%06d %d %d %d\n", sent_id,
            _file_id, start_pos, pre_len);
    //sent_id++;
    start_pos += pre_len;
    //
    for (int ii = 0; ii < _sent_num; ii++) {
        int sent_len = _len_vec[ii];
        for (int jj = 0; jj < sent_len; jj++) {
            for (int kk = 0; kk < _feat_dim; kk++) {
                swap32((char*)&_feat_mat_vec[ii][jj][kk]);
            }
            fwrite(&_feat_mat_vec[ii][jj][0], sizeof(float), _feat_dim, fp_feat);
        }
        if (_has_label) {
            fwrite(&_label_vec_vec[ii][0], sizeof(int), sent_len, fp_label);
        }
        fprintf(fp_desc, "%06d %d %d %d\n", sent_id, _file_id, start_pos, sent_len);
        sent_id++;
        start_pos += sent_len;
    }
    _file_id++;

    fclose(fp_feat);
    fp_feat = NULL;
    if (_has_label) {
        fclose(fp_label);
        fp_label = NULL;
    }
    fclose(fp_desc);
    fp_desc = NULL;
}

void GenerateFeat::read()
{
    char feat_file[1024], label_file[1024], desc_file[1024];
    //char file_name[1024];
    char *temp = NULL;
    while(!feof(_fp_data_lst)) {
        fgets(feat_file, 1024, _fp_data_lst);
        temp = strtok(feat_file, "\r\n");
        if (temp == NULL) {
            continue;
        }
        strcpy(feat_file, temp);

        if (_has_label) {
            fgets(label_file, 1024, _fp_data_lst);
            temp = strtok(label_file, "\r\n");
            if (temp == NULL) {
                continue;
            }
            strcpy(label_file, temp);
        }

        fgets(desc_file, 1024, _fp_data_lst);
        temp = strtok(desc_file, "\r\n");
        if (temp == NULL) {
            continue;
        }
        strcpy(desc_file, temp);
        //
        clear();
        read(feat_file, label_file, desc_file);
    }
    temp = NULL;
}

void GenerateFeat::read(char *feat_file, char *label_file, char *desc_file)
{
    FILE *fp_feat  = NULL;
    FILE *fp_label = NULL;
    FILE *fp_desc  = NULL;
    fp_feat = fopen(feat_file, "rb");
    if (fp_feat == NULL) {
        printf("Cannot open file %s to write!\n", feat_file);
    }
    if (_has_label) {
        fp_label = fopen(label_file, "rb");
        if (fp_label == NULL) {
            printf("Cannot open file %s to write!\n", label_file);
        }
    }
    fp_desc = fopen(desc_file, "rt");
    if (fp_desc == NULL) {
        printf("Cannot open file %s to write!\n", desc_file);
    }

    int head_size = 0;
    head_size = get_header_size(feat_file, true);

    char str[4096];
    char file_name[4096];
    int  speaker_id = 0;
    int  start_pos  = 0;
    int  sent_len   = 0;
    fgets(str, 4096, fp_desc);  // read a line first, to jump the head
    while(!feof(fp_desc)) {
        fgets(str, 4096, fp_desc);
        int item_num = sscanf("%s %d %d %d %d",
                              file_name, &speaker_id, &start_pos, &sent_len);

        if (item_num != 4) {
            printf("desc warning: %s\n", str);
            continue;
        }

        fseek(fp_feat, start_pos * _feat_dim * sizeof(float) + head_size, SEEK_SET);
        for (int ii = 0; ii < sent_len; ii++) {
            fread(&_feat_mat_vec[_sent_num][ii][0], sizeof(float), _feat_dim, fp_feat);
            for (int jj = 0; jj < _feat_dim; jj++) {
                swap32((char *)&_feat_mat_vec[_sent_num][ii][jj]);
            }
        }
        if (_has_label) {
            fseek(fp_label, start_pos * sizeof(int), SEEK_SET);
            fread(&_label_vec_vec[_sent_num][0], sizeof(int), sent_len, fp_label);
        }
        _sent_num++;

        if (_sent_num == _max_sent_num) {
            printf("too many sentence in one file!\n");
            break;
        }
    }

    fclose(fp_feat);
    fp_feat = NULL;
    if (_has_label) {
        fclose(fp_label);
        fp_label = NULL;
    }
    fclose(fp_desc);
    fp_desc = NULL;
}

int GenerateFeat::get_header_size(const char* file, bool is_print)
{
    HtkHeader hdr;
    int head_size = 0;
    FILE* fp = fopen(file, "rb");
    if (fp == NULL) {
        printf("Cannot open file %s to read!\n", file);
    }

    fseek(fp, 0, SEEK_END);
    int n_byte = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    fread(&hdr, sizeof(HtkHeader), 1, fp);
    fclose(fp);

    swap32((char*)&hdr.htk_sample_kind);
    swap32((char*)&hdr.htk_samples_period);
    swap16((char*)&hdr.htk_samples_size);
    swap16((char*)&hdr.htk_samples_kind);

    if (n_byte != (int)sizeof(hdr) + hdr.htk_sample_kind * hdr.htk_samples_size) {
        head_size = 0;
    } else {
        head_size = sizeof(hdr);
    }
    if (is_print) {
        printf("file: %s has %d bytes head-info.\n", file, head_size);
    }
    return head_size;
}
