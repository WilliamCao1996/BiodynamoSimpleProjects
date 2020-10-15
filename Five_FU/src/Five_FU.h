// -----------------------------------------------------------------------------
//
// Copyright (C) The BioDynaMo Project.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
//
// See the LICENSE file distributed with this work for details.
// See the NOTICE file distributed with this work for additional information
// regarding copyright ownership.
//
// -----------------------------------------------------------------------------
#ifndef FIVE_FU_H_
#define FIVE_FU_H_


#include "biodynamo.h"
#include<cmath>
#include "core/substance_initializers.h"

namespace bdm {


struct LinearConcentration {
    double startvalue_;
    double endvalue_;
    double startpos_;
    double endpos_;
    double slope_;
    double intercept_;
    uint8_t axis_;
    LinearConcentration(double startvalue, double endvalue, double startpos, double endpos, uint8_t axis) {
        startvalue_ = startvalue;
        endvalue_ = endvalue;
        startpos_ = startpos;
        endpos_ = endpos;
        axis_ = axis;
        slope_ = (endvalue_ - startvalue_) / (endpos_ - startpos_);
        intercept_ = startvalue_ - (slope_ * startpos_);
    }
    double operator()(double x, double y, double z) {
        switch(axis_) {
            case Axis::kXAxis: return (slope_ * x) + intercept_;
            case Axis::kYAxis: return (slope_ * y) + intercept_;
            case Axis::kZAxis: return (slope_ * z) + intercept_;
            default: throw std::logic_error("You have chosen an non-existing axis!");
        }
    }
};

enum Substances { kSubstance };








// Define my custom cell MyCell, which extends Cell by adding extra data
// members: cell_color and can_divide
class MyCell : public Cell {  // our object extends the Cell object
                              // create the header with our new data member
  BDM_SIM_OBJECT_HEADER(MyCell, Cell, 1,);

 public:
  MyCell() {}
  explicit MyCell(const Double3& position) : Base(position) {}

  /// If MyCell divides, daughter 2 copies the data members from the mother
  MyCell(const Event& event, SimObject* other, uint64_t new_oid = 0)
      : Base(event, other, new_oid) {
  }

  /// If a cell divides, daughter keeps the same state from its mother.
  void EventHandler(const Event& event, SimObject* other1,
                    SimObject* other2 = nullptr) override {
    Base::EventHandler(event, other1, other2);
  }



};

// Define Chemical Drug Biology Module
struct ChemicalDrugBM : public BaseBiologyModule {
 public:
  ChemicalDrugBM() : BaseBiologyModule(gAllEventIds) {}

  ChemicalDrugBM(const Event& event, BaseBiologyModule* other,
                      uint64_t new_oid = 0) : ChemicalDrugBM() {}

  BaseBiologyModule* GetInstance(const Event& event, BaseBiologyModule* other,
                                 uint64_t new_oid = 0) const override {
    return new ChemicalDrugBM(event, other, new_oid);
  }

  BaseBiologyModule* GetCopy() const override {
    return new ChemicalDrugBM(*this);
  }

  void Run(SimObject* so) override {
    auto* sim = Simulation::GetActive();
    auto* random = sim->GetRandom();
    auto* cell = bdm_static_cast<Cell*>(so);
    auto* rm = sim->GetResourceManager();
    static auto* kDg = rm->GetDiffusionGrid(kSubstance);
    const auto& current_position = so->GetPosition();  // get current cell postion
    double current_concentration;  // get concentraion at current cell position
    current_concentration = kDg->GetConcentration(current_position);
            
            
            
    // Consider one timestep as an hour, one day have 24 timesteps
    // H is abbr for hours
    // P is proportion for remaining cells
    double P;
    
    P = pow(2.71828,(8.77735*0.0001-0.0025*log(current_concentration+0.32518)));
    if (current_concentration > 1.09)
         {
           if (random->Uniform(0.0, 1.0) > P) 
            {
             cell->RemoveFromSimulation();
             return;
            } 
         }
    else {
           if(random->Uniform(0.0, 1.0) < P-1)
           {
             cell->Divide();
           }
         }
    
    
    H++;
    }

 private:
  unsigned H = 0;
  BDM_CLASS_DEF_OVERRIDE(ChemicalDrugBM, 1);
};


inline int Simulate(int argc, const char** argv) {
  auto set_param = [](Param* param) {
    param->bound_space_ = true;
    param->min_bound_ = -150;
    param->max_bound_ = 150;  // cube of 300*300*300
  };

  Simulation simulation(argc, argv, set_param);
  auto* rm = simulation.GetResourceManager();
  auto* param = simulation.GetParam();
  auto* myrand = simulation.GetRandom();

  size_t nb_of_cells = 10000;  // number of cells in the simulation
  double x_coord, y_coord, z_coord;

  for (size_t i = 0; i < nb_of_cells; ++i) {
    // the simulation starts with a cell cube of 300*300*300
    // random double between 0 and 100
    x_coord = myrand->Uniform(param->min_bound_, param->max_bound_);
    y_coord = myrand->Uniform(param->min_bound_, param->max_bound_);
    z_coord = myrand->Uniform(param->min_bound_, param->max_bound_);

    // creating the cell at position x, y, z
    MyCell* cell = new MyCell({x_coord, y_coord, z_coord});
    // set cell parameters
    cell->SetDiameter(7.5);
    cell->AddBiologyModule(new ChemicalDrugBM());
    rm->push_back(cell);  // put the created cell in our cells structure
  }

    // Define the substances in our simulation
    // Order: substance id, substance_name, diffusion_coefficient, decay_constant,
    // resolution
    ModelInitializer::DefineSubstance(kSubstance, "5-FU", 0, 0, 20);

    // Init substance with linear concentration distribution
    //  LinearGradiend(double startvalue, double endvalue, double startpos, double endpos, uint8_t axis)
    double concentration = 500;
    ModelInitializer::InitializeSubstance(kSubstance, "5-FU",
                                          LinearConcentration(concentration, concentration-0.01*concentration, 0, 100, Axis::kZAxis));

  // Run simulation for 72 hours
  std::cout <<"Drug name: 5-FU "<<"Drug concentration: "<< concentration<< " uM"<<std::endl;
  std::cout <<"Initial cell numbers: "<< nb_of_cells << std::endl;
  simulation.GetScheduler()->Simulate(24);
  std::cout <<"cell numbers after 24h of drug treatment: " <<rm->GetNumSimObjects() << std::endl;
  simulation.GetScheduler()->Simulate(48);
  std::cout <<"cell numbers after 72h of drug treatment: " <<rm->GetNumSimObjects() << std::endl;
  return 0;
}

}  // namespace bdm


#endif  // FIVE_FU_H_
