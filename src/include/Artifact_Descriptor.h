#ifndef BANDCAMP_ARTIFACT_DESCRIPTOR_H_
#define BANDCAMP_ARTIFACT_DESCRIPTOR_H_


#include <iostream>
#include <string>
#include <time.h>
#include <vector>

class Artifact_Descriptor {
private:
    std::string get_name();
    std::string get_ruler();
    std::vector<std::string> historical_figures;
    
public:
    Artifact_Descriptor();
    std::string gen_item_description();
    std::string get_first_descriptor();
    std::string get_second_descriptor();
};

#endif // BANDCAMP_ARTIFACT_DESCRIPTOR_H_
