/*
  ManagEZ v1.0 - Processor Header

  Full-featured task management VST plugin
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>

// Priority levels for tasks
enum class Priority { None = 0, Low = 1, Medium = 2, High = 3 };

// Task categories
enum class Category { General, Mix, Master, Record, Release };

// Task data structure
struct Task {
  int id;
  juce::String text;
  bool completed;
  Priority priority;
  Category category;

  Task()
      : id(0), completed(false), priority(Priority::None),
        category(Category::General) {}
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

  const juce::String getName() const override { return "ManagEZ"; }
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
  void editTask(int index, const juce::String &newText);
  void removeTask(int index);
  void toggleTask(int index);
  void setTaskPriority(int index, Priority priority);
  void setTaskCategory(int index, Category category);
  void reorderTask(int fromIndex, int toIndex);

  // Template management
  void loadTemplate(const juce::String &templateName);
  void clearAllTasks();

  // Getters
  const std::vector<Task> &getTasks() const { return tasks; }
  int getCompletedCount() const;
  int getTotalCount() const { return static_cast<int>(tasks.size()); }

  // Listener for UI updates
  class Listener {
  public:
    virtual ~Listener() = default;
    virtual void tasksChanged() = 0;
  };

  void addListener(Listener *l);
  void removeListener(Listener *l);

private:
  std::vector<Task> tasks;
  std::vector<Listener *> listeners;
  int nextTaskId;

  void notifyListeners();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleChecklistProcessor)
};
