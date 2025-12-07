/*
  Minimal VST3 Checklist Plugin - Processor Implementation
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

SimpleChecklistProcessor::SimpleChecklistProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)) {
  // Start with one example task
  addTask("Add your first task!");
}

SimpleChecklistProcessor::~SimpleChecklistProcessor() {}

void SimpleChecklistProcessor::prepareToPlay(double, int) {}
void SimpleChecklistProcessor::releaseResources() {}

void SimpleChecklistProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                            juce::MidiBuffer &) {
  // Pass-through audio (no processing)
  juce::ScopedNoDenormals noDenormals;
  for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels();
       ++i)
    buffer.clear(i, 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor *SimpleChecklistProcessor::createEditor() {
  return new SimpleChecklistEditor(*this);
}

//==============================================================================
// Task Management

void SimpleChecklistProcessor::addTask(const juce::String &text) {
  if (text.trim().isEmpty())
    return;
  tasks.push_back(Task(text));
  notifyListeners();
}

void SimpleChecklistProcessor::removeTask(int index) {
  if (index >= 0 && index < (int)tasks.size()) {
    tasks.erase(tasks.begin() + index);
    notifyListeners();
  }
}

void SimpleChecklistProcessor::toggleTask(int index) {
  if (index >= 0 && index < (int)tasks.size()) {
    tasks[index].completed = !tasks[index].completed;
    notifyListeners();
  }
}

void SimpleChecklistProcessor::notifyListeners() {
  listeners.call([](Listener &l) { l.tasksChanged(); });
}

//==============================================================================
// State Persistence

void SimpleChecklistProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  auto xml = std::make_unique<juce::XmlElement>("Tasks");

  for (const auto &task : tasks) {
    auto *taskXml = xml->createNewChildElement("Task");
    taskXml->setAttribute("text", task.text);
    taskXml->setAttribute("completed", task.completed);
  }

  copyXmlToBinary(*xml, destData);
}

void SimpleChecklistProcessor::setStateInformation(const void *data,
                                                   int sizeInBytes) {
  auto xml = getXmlFromBinary(data, sizeInBytes);

  if (xml && xml->hasTagName("Tasks")) {
    tasks.clear();

    for (auto *taskXml : xml->getChildIterator()) {
      if (taskXml->hasTagName("Task")) {
        Task task;
        task.text = taskXml->getStringAttribute("text");
        task.completed = taskXml->getBoolAttribute("completed");
        tasks.push_back(task);
      }
    }

    notifyListeners();
  }
}

//==============================================================================
// Plugin instantiation

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new SimpleChecklistProcessor();
}
