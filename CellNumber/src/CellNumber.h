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
#ifndef CELLNUMBER_H_
#define CELLNUMBER_H_

#include "biodynamo.h"
#include <ctime>

namespace bdm {

// Define my custom cell MyCell, which extends Cell by adding extra data
// members: can_divide
class MyCell : public Cell {  // our object extends the Cell object
                              // create the header with our new data member
  BDM_SIM_OBJECT_HEADER(MyCell, Cell, 1, can_divide_);

 public:
  MyCell() {}
  explicit MyCell(const Double3& position) : Base(position) {}

  /// If MyCell divides, daughter 2 copies the data members from the mother
  MyCell(const Event& event, SimObject* other, uint64_t new_oid = 0)
      : Base(event, other, new_oid) {
    if (auto* mother = dynamic_cast<MyCell*>(other)) {
      if (event.GetId() == CellDivisionEvent::kEventId) {
        // the daughter will be able to divide
        can_divide_ = true;
      } else {
        can_divide_ = mother->can_divide_;
      }
    }
  }

  /// If a cell divides, daughter keeps the same state from its mother.
  void EventHandler(const Event& event, SimObject* other1,
                    SimObject* other2 = nullptr) override {
    Base::EventHandler(event, other1, other2);
  }

  // getter and setter for our new data member
  void SetCanDivide(bool d) { can_divide_ = d; }
  bool GetCanDivide() const { return can_divide_; }

 private:
  // declare new data member and define their type
  // private data can only be accessed by public function and not directly
  bool can_divide_;
};

// Define growth behaviour
struct GrowthModule : public BaseBiologyModule {
  BDM_STATELESS_BM_HEADER(GrowthModule, BaseBiologyModule, 1);

  GrowthModule() : BaseBiologyModule(gAllEventIds) {}

  /// Empty default event constructor, because GrowthModule does not have state.
  template <typename TEvent, typename TBm>
  GrowthModule(const TEvent& event, TBm* other, uint64_t new_oid = 0)
      : BaseBiologyModule(event, other, new_oid) {}

  /// event handler not needed, because Chemotaxis does not have state.

  void Run(SimObject* so) override {
    if (auto* cell = dynamic_cast<MyCell*>(so)) {
      if (cell->GetDiameter() < 8) {
        // Here 400 is the speed and the change to the volume is based on the
        // simulation time step.
        // The default here is 0.01 for timestep, not 1.
        cell->ChangeVolume(400);

      } else {
        // Here below is a random seed link to the clock.
        // The random result change with time.
        // If you run this simulation for multiple times, you will get different result.
        auto* random = Simulation::GetActive()->GetRandom();
        random->SetSeed(((unsigned int)std::time(0))*t);

        if (cell->GetCanDivide() && random->Uniform(0, 1) > 0.1) {
          cell->Divide();
        } else {
          cell->SetCanDivide(false);  // this cell won't divide anymore
        }
      }
    }
    // t is how many simulation timesteps passed.
    // s in the SetSeed() means there will be a new seed for each timestep.

                t++;
  }
       private:
  unsigned t = 1;
};

inline int Simulate(int argc, const char** argv) {
  auto set_param = [](Param* param) {
    param->bound_space_ = true;
    param->min_bound_ = 0;
    param->max_bound_ = 300;  // cube of 300*300*300
  };

  Simulation simulation(argc, argv, set_param);
  auto* rm = simulation.GetResourceManager();


  // create a cancerous cell, containing the biology module GrowthModule
  // cell diameter starts at 6.35 and cell division happen when diameter reach 8
  // Because the two spheres with a diameter of 6.35 are the same size as a sphere with a diameter of 8.
  MyCell* cell = new MyCell({150, 150, 150});
  cell->SetDiameter(6.35);
  cell->SetCanDivide(true);
  cell->AddBiologyModule(new GrowthModule());
  rm->push_back(cell);  // put the created cell in our cells structure

  // Run simulation

  int i;
    for (i = 0; i < 50; ++i){
  simulation.GetScheduler()->Simulate(10);
  std::cout << (i+1)*10 <<" timesteps past, number of cancer cells: " << rm->GetNumSimObjects() << std::endl;
  }

  std::cout << "In this simulation, cancer cells have 90 percent chance of division" << std::endl;
  std::cout << "Simulation completed successfully!" << std::endl;
  return 0;
}

}  // namespace bdm


#endif  // CELLNUMBER_H_
