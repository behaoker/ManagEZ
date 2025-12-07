/*
  ManagEZ v1.0 - Processor Implementation
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

SimpleChecklistProcessor::SimpleChecklistProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      nextTaskId(1) {}

SimpleChecklistProcessor::~SimpleChecklistProcessor() {}

void SimpleChecklistProcessor::prepareToPlay(double, int) {}
void SimpleChecklistProcessor::releaseResources() {}

void SimpleChecklistProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                            juce::MidiBuffer &) {
  // Pass-through audio (no processing)
  for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
    buffer.clear(i, 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor *SimpleChecklistProcessor::createEditor() {
  return new SimpleChecklistEditor(*this);
}

void SimpleChecklistProcessor::addTask(const juce::String &text) {
  Task task;
  task.id = nextTaskId++;
  task.text = text;
  task.completed = false;
  task.priority = Priority::None;
  task.category = Category::General;
  tasks.push_back(task);
  notifyListeners();
}

void SimpleChecklistProcessor::editTask(int index,
                                        const juce::String &newText) {
  if (index >= 0 && index < static_cast<int>(tasks.size())) {
    tasks[index].text = newText;
    notifyListeners();
  }
}

void SimpleChecklistProcessor::removeTask(int index) {
  if (index >= 0 && index < static_cast<int>(tasks.size())) {
    tasks.erase(tasks.begin() + index);
    notifyListeners();
  }
}

void SimpleChecklistProcessor::toggleTask(int index) {
  if (index >= 0 && index < static_cast<int>(tasks.size())) {
    tasks[index].completed = !tasks[index].completed;
    notifyListeners();
  }
}

void SimpleChecklistProcessor::setTaskPriority(int index, Priority priority) {
  if (index >= 0 && index < static_cast<int>(tasks.size())) {
    tasks[index].priority = priority;
    notifyListeners();
  }
}

void SimpleChecklistProcessor::setTaskCategory(int index, Category category) {
  if (index >= 0 && index < static_cast<int>(tasks.size())) {
    tasks[index].category = category;
    notifyListeners();
  }
}

void SimpleChecklistProcessor::reorderTask(int fromIndex, int toIndex) {
  if (fromIndex >= 0 && fromIndex < static_cast<int>(tasks.size()) &&
      toIndex >= 0 && toIndex < static_cast<int>(tasks.size()) &&
      fromIndex != toIndex) {
    Task task = tasks[fromIndex];
    tasks.erase(tasks.begin() + fromIndex);
    tasks.insert(tasks.begin() + toIndex, task);
    notifyListeners();
  }
}

void SimpleChecklistProcessor::clearAllTasks() {
  tasks.clear();
  notifyListeners();
}

void SimpleChecklistProcessor::loadTemplate(const juce::String &templateName) {
  clearAllTasks();

  if (templateName == "Mixing") {
    addTask("Set reference track");
    tasks.back().category = Category::Mix;
    addTask("Check all levels (-6dB headroom)");
    tasks.back().category = Category::Mix;
    addTask("EQ each track");
    tasks.back().category = Category::Mix;
    addTask("Compress where needed");
    tasks.back().category = Category::Mix;
    addTask("Pan placement");
    tasks.back().category = Category::Mix;
    addTask("Add reverb/delay");
    tasks.back().category = Category::Mix;
    addTask("Automation passes");
    tasks.back().category = Category::Mix;
    addTask("Check in mono");
    tasks.back().category = Category::Mix;
    addTask("Bus processing");
    tasks.back().category = Category::Mix;
    addTask("Final limiter check");
    tasks.back().category = Category::Mix;
  } else if (templateName == "Mastering") {
    addTask("Load reference track");
    tasks.back().category = Category::Master;
    tasks.back().priority = Priority::High;
    addTask("Set monitoring level");
    tasks.back().category = Category::Master;
    addTask("EQ adjustments");
    tasks.back().category = Category::Master;
    addTask("Multiband compression");
    tasks.back().category = Category::Master;
    addTask("Limiting (-0.1dB peak)");
    tasks.back().category = Category::Master;
    tasks.back().priority = Priority::High;
    addTask("Check LUFS (-14 for streaming)");
    tasks.back().category = Category::Master;
    tasks.back().priority = Priority::High;
    addTask("Export WAV 24-bit");
    tasks.back().category = Category::Master;
    addTask("Export MP3 320kbps");
    tasks.back().category = Category::Master;
    addTask("Add metadata");
    tasks.back().category = Category::Master;
  } else if (templateName == "Recording") {
    addTask("Setup microphones");
    tasks.back().category = Category::Record;
    addTask("Check input levels");
    tasks.back().category = Category::Record;
    tasks.back().priority = Priority::High;
    addTask("Set monitoring mix");
    tasks.back().category = Category::Record;
    addTask("Enable click track");
    tasks.back().category = Category::Record;
    addTask("Create headphone mix");
    tasks.back().category = Category::Record;
    addTask("Arm tracks");
    tasks.back().category = Category::Record;
    addTask("Do sound check");
    tasks.back().category = Category::Record;
    tasks.back().priority = Priority::High;
    addTask("Set markers");
    tasks.back().category = Category::Record;
  } else if (templateName == "Release") {
    addTask("Final mix approved");
    tasks.back().category = Category::Release;
    tasks.back().priority = Priority::High;
    addTask("Master approved");
    tasks.back().category = Category::Release;
    tasks.back().priority = Priority::High;
    addTask("Artwork ready (3000x3000)");
    tasks.back().category = Category::Release;
    addTask("Metadata complete");
    tasks.back().category = Category::Release;
    addTask("Upload to distributor");
    tasks.back().category = Category::Release;
    addTask("Schedule release date");
    tasks.back().category = Category::Release;
    addTask("Prepare social posts");
    tasks.back().category = Category::Release;
    addTask("Send to playlist curators");
    tasks.back().category = Category::Release;
  }

  notifyListeners();
}

int SimpleChecklistProcessor::getCompletedCount() const {
  int count = 0;
  for (const auto &task : tasks) {
    if (task.completed)
      count++;
  }
  return count;
}

void SimpleChecklistProcessor::addListener(Listener *l) {
  if (l != nullptr) {
    listeners.push_back(l);
  }
}

void SimpleChecklistProcessor::removeListener(Listener *l) {
  listeners.erase(std::remove(listeners.begin(), listeners.end(), l),
                  listeners.end());
}

void SimpleChecklistProcessor::notifyListeners() {
  for (auto *listener : listeners) {
    if (listener != nullptr) {
      listener->tasksChanged();
    }
  }
}

void SimpleChecklistProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  auto xml = std::make_unique<juce::XmlElement>("Tasks");

  for (const auto &task : tasks) {
    auto *taskXml = xml->createNewChildElement("Task");
    taskXml->setAttribute("id", task.id);
    taskXml->setAttribute("text", task.text);
    taskXml->setAttribute("completed", task.completed);
    taskXml->setAttribute("priority", static_cast<int>(task.priority));
    taskXml->setAttribute("category", static_cast<int>(task.category));
  }

  xml->setAttribute("nextTaskId", nextTaskId);

  copyXmlToBinary(*xml, destData);
}

void SimpleChecklistProcessor::setStateInformation(const void *data,
                                                   int sizeInBytes) {
  auto xml = getXmlFromBinary(data, sizeInBytes);

  if (xml && xml->hasTagName("Tasks")) {
    tasks.clear();

    nextTaskId = xml->getIntAttribute("nextTaskId", 1);

    for (auto *taskXml : xml->getChildIterator()) {
      if (taskXml->hasTagName("Task")) {
        Task task;
        task.id = taskXml->getIntAttribute("id");
        task.text = taskXml->getStringAttribute("text");
        task.completed = taskXml->getBoolAttribute("completed");
        task.priority =
            static_cast<Priority>(taskXml->getIntAttribute("priority"));
        task.category =
            static_cast<Category>(taskXml->getIntAttribute("category"));
        tasks.push_back(task);
      }
    }

    notifyListeners();
  }
}

// Required for plugin creation
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new SimpleChecklistProcessor();
}
