#include "include/K3Buffer.h"

K3Buffer::~K3Buffer()
{
     struct Feature* f = this->head;
     while (f != nullptr)
     {
          this->info(0, f->name);
          delete f->buffer;
          this->info(0, "deleted\n");
          f = f->next;
     }
}

void K3Buffer::info(float value, const char* description = "")
{
     printf("K3Buffer %f %s", value, description);
}

struct Feature* K3Buffer::emerge(const char* name)
{
     struct Feature* feature = new Feature;
     feature->name = name;
     feature->mini = +1e10;
     feature->maxi = -1e10;
     feature->buffer = new std::vector<float>;
     feature->next = nullptr;

     struct Feature* f = this->head;
     if (f != nullptr)
     {
          while (f->next != NULL) f = f->next;
          f->next = feature;
     }
     else
     {
          this->head = feature;
     }

     this->info(1, name);
     this->info(1, "created\n");
     return feature;
}

struct Feature* K3Buffer::node(const char* name)
{
     struct Feature* f = this->head;
     int count = 0;
     while (f != nullptr)
     {
          if (f->name == name) return f;
          f = f->next;
          count ++;
     }

     return this->emerge(name);
}

void K3Buffer::setminmax(struct Feature* ens, float cur)
{
     float minrec = ens->mini;
     float maxrec = ens->maxi;
     ens->maxi = cur > maxrec ? cur : maxrec;
     ens->mini = cur < minrec ? cur : minrec;
}

std::vector<float>* K3Buffer::get(const char* name)
{
     struct Feature* ens = this->node(name);
     return ens->buffer;
}

void K3Buffer::fill(const char* name, float value)
{
     struct Feature* ens = this->node(name);
     std::vector<float>* floatVect = ens->buffer;
     floatVect->push_back(value);
     int fize = floatVect->size();
     if (fize > this->buffer_size) floatVect->erase(floatVect->begin());

     this->setminmax(ens, value);
}

float K3Buffer::min(std::vector<float>* floatVect)
{
     return *std::min_element(floatVect->begin(), floatVect->end());
}

float K3Buffer::max(std::vector<float>* floatVect)
{
     return *std::max_element(floatVect->begin(), floatVect->end());
}

void K3Buffer::statistique(std::vector<float> histogram,
                           int N, float bmin, float bmax,
                           float* m, float* d)
{

     // N is the number of measurements with values in the range [bmin; nmax]
     int n = histogram.size(); // the number of histogram bins
     float b = (bmax - bmin) / n; // constant histogram bin width 
     float W = 0.0f; // sum of weights
     float S = 0.0f; // sum of squared differences

     for (int i = 0; i < n; i++)
     {
          int x = histogram[i]; // bin count
          float c = b * (1. / 2 + i) + bmin; // bin center
          float w = x * c; // bin weight
          W += w;
     }

     *m = W / N; // calculated mean

     for (int i = 0; i < n; i++)
     {
          int x = histogram[i];
          float c = b * (1. / 2 + i) + bmin;
          float s = (c - *m) * (c - *m); // the squared difference
          S += x * s;
     }

     float v = S / N; // calculated variation
     *d = std::sqrt(v); // calculated standard deviation     
}

void K3Buffer::build(const char* name, std::vector<float>* histogram,
                     float* hmin, float* hmax, float* hmean, float* hstdev,
                     float* bmin, float* bmax, float* cur)
{
     std::vector<float>* floatVect = this->get(name);
     *bmin = this->min(floatVect);
     *bmax = this->max(floatVect);
     *cur = floatVect->back();
     float buff_size = floatVect->size();

     float hist_size = histogram->size();
     float bin_width = (*bmax - *bmin) / hist_size;

     for (int i = 0; i < buff_size; i++)
     {
          float value = (*floatVect)[i];
          int bin_index = (int)((value - *bmin) / bin_width);
          if (bin_index >= 0 && bin_index < hist_size)
          {
               float old_value = (*histogram)[bin_index];
               (*histogram)[bin_index] = old_value + 1;
          }
     }

     this->statistique(*histogram, buff_size, *bmin, *bmax, hmean, hstdev);
     *hmin = this->min(histogram);
     *hmax = this->max(histogram);
}

void K3Buffer::reset(std::vector<float>* floatVect)
{
     if (floatVect != nullptr) floatVect->clear();
}

void K3Buffer::dump(std::vector<float>* floatVect)
{
     this->info(floatVect->back());
}

void K3Buffer::dump()
{
     struct Feature* node = this->head;
     while (node != NULL)
     {
          this->info(3, node->name);
          this->dump(node->buffer);
          node = node->next;
     }
}

void K3Buffer::reset()
{
     struct Feature* node = this->head;
     while (node != NULL)
     {
          this->reset(node->buffer);
          node = node->next;
     }
}
volatile int written = 0;
volatile int writtenMax = 0;
// (title, cur, hmean, hstdev, siunit, bmin, bmax);
const char* K3Buffer::overtextHist(const char* title, float cur, float hmean, float hstdev, const char* sunit, float bmin, float bmax)
{
     static char ot[78];
     written = snprintf(ot, sizeof(ot), "%-17s %6.2f %6.2f mean %6.2f sdev %2s %6.2f min %6.2f max",
              title, cur, hmean, hstdev, sunit, bmin, bmax);
              if (written > writtenMax) writtenMax = written;
     return ot;
}

// (title, cur, min, max, siunit, ens->mini, ens->maxi);
const char* K3Buffer::overtextTime(const char* title, float cur, float min, float max, const char* sunit, float mini, float maxi)
{
     static char ot[78];
     written = snprintf(ot, sizeof(ot), "%-17s %6.2f %6.2f %6.2f %2s",
              title, cur, min, max, sunit); // , mini, maxi);
              if (written > writtenMax) writtenMax = written;
     return ot;
}
