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
#ifndef CELLDISTRIBUTION_H_
#define CELLDISTRIBUTION_H_
#include "biodynamo.h"

namespace bdm {

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

// Define growth behaviour
struct GrowthModule : public BaseBiologyModule {
  BDM_STATELESS_BM_HEADER(GrowthModule, BaseBiologyModule, 1);

  GrowthModule() : BaseBiologyModule(gAllEventIds) {}

  /// Empty default event constructor, because GrowthModule does not have state.
  template <typename TEvent, typename TBm>
  GrowthModule(const TEvent& event, TBm* other, uint64_t new_oid = 0)
      : BaseBiologyModule(event, other, new_oid) {}


  void Run(SimObject* so) override {
    if (auto* cell = dynamic_cast<MyCell*>(so)) {
      if (cell->GetDiameter() < 8) {
        auto* random = Simulation::GetActive()->GetRandom();

        // Here 400 is the speed and the change to the volume is based on the
        // simulation time step.
        // The default here is 0.01 for timestep, not 1.
        cell->ChangeVolume(400);
        
        // Here below is a random seed link to the clock.
        // The random result change with time.
        // If you run this simulation for multiple times, you will get different results.
        
        random->SetSeed(((unsigned int)std::time(0))*t);
        // create an array of 3 random numbers between -2 and 2
        Double3 cell_movements{random->Uniform(-2, 2), random->Uniform(-2, 2), random->Uniform(-2, 2)};
        // update the cell mass location, ie move the cell
        cell->UpdatePosition(cell_movements);
      } 
      else {
              cell->Divide();
           }
    }
    // t is how many simulation timesteps passed.
    // t in the SetSeed() means there will be a new seed for each timestep.
        t++;
  }
   private:
  unsigned t = 0;
};

inline int Simulate(int argc, const char** argv) {
  auto set_param = [](Param* param) {
    param->bound_space_ = true;
    param->min_bound_ = 0;
    param->max_bound_ = 300;  // cube of 100*100*100
  };

  Simulation simulation(argc, argv, set_param);
  auto* rm = simulation.GetResourceManager();


  // create a cancerous cell, containing the biology module GrowthModule
  MyCell* cell = new MyCell({150, 150, 150});
  cell->SetDiameter(6);
  cell->AddBiologyModule(new GrowthModule());
  rm->push_back(cell);  // put the created cell in our cells structure


  // Run simulation
  simulation.GetScheduler()->Simulate(500);
  
  long unsigned int num_allcell;
  num_allcell = rm->GetNumSimObjects();

  for (size_t i = 0; i < num_allcell-1; ++i){
  	MyCell* cell = (MyCell*) rm->GetSimObject((const bdm::SoUid) i);
        std::cout << "Cell UID: "<< cell->GetUid()<< " Cell x coordinate: "<< cell->GetPosition()[0]<< std::endl;
  }
    
  std::cout << "In this simulation, cells migrate ramdomly from (-2,-2,-2) to (2,2,2) every timestep" << std::endl;
  std::cout << "number of cells after 500 timesteps: " << rm->GetNumSimObjects() << std::endl;
  return 0;
}

}  // namespace bdm


#endif  // CELLDISTRIBUTION_H_
