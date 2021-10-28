#ifndef CLASS_POPULATION_H
#define CLASS_POPULATION_H

#include "vulkan_header.h"
#include "brush.h"
#include <glm/glm.hpp>
#include "vkcpp/stdafx.h"

namespace painting
{
    struct PopulationComponent
    {
        glm::vec2 offset{};
        glm::vec2 extent{};
        uint32_t min_population_size{0};
        uint32_t attributes_size{0};
        uint32_t stage_count{0};
    };
    /**
     * TODO: alpha sort
     */
    class Population
    {
    private:
        std::vector<std::unique_ptr<BrushAttributes>> population_;
        PopulationComponent component_{};
        glm::vec2 scale_range_{};
        BrushAttributes::Probablity probablity_{};
        double best_fit_{0.0};

    public:
        Population(const glm::vec2 &offset,
                   const glm::vec2 &extent,
                   const glm::vec2 &scale_range,
                   const BrushAttributes::Probablity &probablity,
                   uint32_t min_population_size,
                   uint32_t attributes_size);

        ~Population();

        PopulationComponent &get_mutable_component()
        {
            return component_;
        }
        BrushAttributes *get(int idx)
        {
            return population_[idx].get();
        }
        BrushAttributes *top()
        {
            return population_[0].get();
        }
        double &get_mutable_fitness(int idx)
        {
            return population_[idx]->get_mutable_fitness();
        }
        int get_size()
        {
            return population_.size();
        }
        void set_best(double fit)
        {
            best_fit_ = fit;
        }
        double get_best()
        {
            return best_fit_;
        }

        void sort();
        void push_back(int count);
        void pop_back();
        void next_stage();
    };
}

#endif
