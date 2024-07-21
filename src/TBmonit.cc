#include "TBmonit.h"
#include "TBmid.h"
#include "TBevt.h"
#include "TBread.h"
#include "TButility.h"

#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <chrono>

#include "TFile.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraph.h"

template <typename T>
TBmonit<T>::TBmonit(const std::string &fConfig_, int fRunNum_)
    : fConfig(TBconfig(fConfig_)), fRunNum(fRunNum_), fMaxEvent(-1), fMaxFile(-1)
{
  fIsLive = false;
}

template <typename T>
void TBmonit<T>::Loop()
{
  TBplotengine enginePlot = TBplotengine(fConfig.GetConfig()["ModuleConfig"], fRunNum, TButility());
  TBread<TBwaveform> readerWave = TBread<TBwaveform>(fRunNum, fMaxEvent, fMaxFile, fIsLive, "/Users/khwang/scratch/TB2024/dev_240715/TB2024/sample_data", std::vector<int>{1, 2});

  std::chrono::time_point time_begin = std::chrono::system_clock::now();

  int entries = fMaxEvent;
  if (fMaxEvent == -1)
    entries = readerWave.GetMaxEvent();

  std::cout << " " << std::endl;
  for (int i = 0; i < entries; i++)
  {
    if (i > 0 && i % 10 == 0)
    {
      std::chrono::duration time_taken = std::chrono::system_clock::now() - time_begin; // delete
      float percent_done = 1. * i / (float)entries;
      std::chrono::duration time_left = time_taken * (1 / percent_done - 1);
      std::chrono::minutes minutes_left = std::chrono::duration_cast<std::chrono::minutes>(time_left);
      std::chrono::seconds seconds_left = std::chrono::duration_cast<std::chrono::seconds>(time_left - minutes_left);
      std::cout << "\r\033[F"
                << " " << i << " events  " << minutes_left.count() << ":";
      printf("%02d left   %.1f%%            \n", int(seconds_left.count()), percent_done * 100);
    }

    TBevt<TBwaveform> aEvent;
    aEvent = readerWave.GetAnEvent();
    enginePlot.Fill(aEvent);
  }

  enginePlot.SaveAs("DQM_Run" + std::to_string(fRunNum));

  std::chrono::duration time_taken = std::chrono::system_clock::now() - time_begin; // delete
  std::chrono::minutes minutes_taken = std::chrono::duration_cast<std::chrono::minutes>(time_taken);
  std::chrono::seconds seconds_taken = std::chrono::duration_cast<std::chrono::seconds>(time_taken - minutes_taken);

  std::cout << "Time Taken : " << minutes_taken.count() << ":" << seconds_taken.count() << std::endl;
}

template class TBmonit<TBwaveform>;
template class TBmonit<TBfastmode>;
