/*
  Minimal VST3 Checklist Plugin - Processor Header

  Dead simple implementation with no fancy features.
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>

// Simple task struct
struct Task {
  juce::String text;
  bool completed;

  Task() : completed(false) {}
  Task(const juce::String &t) : text(t), completed(false) {}
};

class SimpleChecklistProcessor : public juce::AudioProcessor {
public:
  SimpleChecklistProcessor();
  ~SimpleChecklistProcessor() override;

  // Required AudioProcessor methods
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override { return true; }

  const juce::String getName() const override { return "Simple Checklist"; }
  bool acceptsMidi() const override { return false; }
  bool producesMidi() const override { return false; }
  double getTailLengthSeconds() const override { return 0.0; }

  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  const juce::String getProgramName(int) override { return {}; }
  void changeProgramName(int, const juce::String &) override {}

  // State persistence
  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  // Task management
  void addTask(const juce::String &text);
  void removeTask(int index);
  void toggleTask(int index);
  const std::vector<Task> &getTasks() const { return tasks; }

  // Listener for UI updates
  class Listener {
  public:
    virtual ~Listener() = default;
    virtual void tasksChanged() = 0;
  };

  void addListener(Listener *l) { listeners.add(l); }
  void removeListener(Listener *l) { listeners.remove(l); }

private:
  std::vector<Task> tasks;
  juce::ListenerList<Listener> listeners;

  void notifyListeners();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleChecklistProcessor)
};
