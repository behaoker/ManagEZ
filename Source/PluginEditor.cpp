/*
  ManagEZ v1.0 - Editor Implementation
*/

#include "PluginEditor.h"
#include "BinaryData.h"

SimpleChecklistEditor::SimpleChecklistEditor(SimpleChecklistProcessor &p)
    : AudioProcessorEditor(&p), processor(p), editingEditor(nullptr),
      editingTaskIndex(-1) {
  setSize(450, 600);

  // Load logo
  logoImage = juce::ImageCache::getFromMemory(BinaryData::icon_png,
                                              BinaryData::icon_pngSize);

  // Search box
  addAndMakeVisible(searchBox);
  searchBox.setMultiLine(false);
  searchBox.setTextToShowWhenEmpty("Search tasks...", juce::Colour(0xff888888));
  searchBox.setColour(juce::TextEditor::backgroundColourId,
                      juce::Colour(0xff2d2d2d));
  searchBox.setColour(juce::TextEditor::textColourId, juce::Colours::white);
  searchBox.setColour(juce::TextEditor::outlineColourId,
                      juce::Colour(0xff404040));
  searchBox.addListener(this);

  // Template selector
  addAndMakeVisible(templateSelector);
  templateSelector.addItem("Load Template...", 1);
  templateSelector.addItem("Mixing Checklist", 2);
  templateSelector.addItem("Mastering Checklist", 3);
  templateSelector.addItem("Recording Prep", 4);
  templateSelector.addItem("Release Checklist", 5);
  templateSelector.setSelectedId(1);
  templateSelector.setColour(juce::ComboBox::backgroundColourId,
                             juce::Colour(0xff2d2d2d));
  templateSelector.setColour(juce::ComboBox::textColourId,
                             juce::Colours::white);
  templateSelector.setColour(juce::ComboBox::outlineColourId,
                             juce::Colour(0xff404040));
  templateSelector.addListener(this);

  // Progress label
  addAndMakeVisible(progressLabel);
  progressLabel.setColour(juce::Label::textColourId, juce::Colours::white);
  progressLabel.setJustificationType(juce::Justification::centredRight);

  // Input box
  addAndMakeVisible(inputBox);
  inputBox.setMultiLine(false);
  inputBox.setReturnKeyStartsNewLine(false);
  inputBox.setTextToShowWhenEmpty("Add a task...", juce::Colour(0xff888888));
  inputBox.setColour(juce::TextEditor::backgroundColourId,
                     juce::Colour(0xff2d2d2d));
  inputBox.setColour(juce::TextEditor::textColourId, juce::Colours::white);
  inputBox.setColour(juce::TextEditor::outlineColourId,
                     juce::Colour(0xff404040));
  inputBox.addListener(this);

  // Add button
  addAndMakeVisible(addButton);
  addButton.setButtonText("Add");
  addButton.setColour(juce::TextButton::buttonColourId,
                      juce::Colour(0xff0078d4));
  addButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
  addButton.addListener(this);

  // Task viewport
  addAndMakeVisible(taskViewport);
  taskViewport.setViewedComponent(&taskContainer, false);
  taskViewport.setScrollBarsShown(true, false);

  processor.addListener(this);
  rebuildTaskList();
}

SimpleChecklistEditor::~SimpleChecklistEditor() {
  processor.removeListener(this);
}

void SimpleChecklistEditor::paint(juce::Graphics &g) {
  g.fillAll(juce::Colour(0xff1e1e1e));

  // Draw logo
  if (logoImage.isValid()) {
    int logoSize = 40;
    int logoX = (getWidth() - logoSize) / 2;
    g.drawImage(logoImage, logoX, 5, logoSize, logoSize, 0, 0,
                logoImage.getWidth(), logoImage.getHeight());
  }

  // Title
  g.setColour(juce::Colours::white);
  g.setFont(juce::Font(16.0f, juce::Font::bold));
  g.drawText("ManagEZ", 0, 50, getWidth(), 20, juce::Justification::centred);
}

void SimpleChecklistEditor::resized() {
  auto area = getLocalBounds().reduced(10);

  area.removeFromTop(75); // Logo + title

  // Search and template row
  auto searchRow = area.removeFromTop(30);
  templateSelector.setBounds(searchRow.removeFromRight(150));
  searchRow.removeFromRight(5);
  searchBox.setBounds(searchRow);

  area.removeFromTop(5);

  // Progress label
  auto progressRow = area.removeFromTop(20);
  progressLabel.setBounds(progressRow);

  area.removeFromTop(10);

  // Input row
  auto inputRow = area.removeFromTop(30);
  addButton.setBounds(inputRow.removeFromRight(60));
  inputRow.removeFromRight(5);
  inputBox.setBounds(inputRow);

  area.removeFromTop(10);

  // Task list
  taskViewport.setBounds(area);
}

void SimpleChecklistEditor::buttonClicked(juce::Button *button) {
  if (button == &addButton) {
    addTaskFromInput();
    return;
  }

  // Delete buttons
  for (int i = 0; i < deleteButtons.size(); ++i) {
    if (button == deleteButtons[i]) {
      processor.removeTask(i);
      return;
    }
  }

  // Checkboxes
  for (int i = 0; i < checkboxes.size(); ++i) {
    if (button == checkboxes[i]) {
      processor.toggleTask(i);
      return;
    }
  }
}

void Simple
ChecklistEditor::textEditorReturnKeyPressed(juce::TextEditor &editor) {
  if (&editor == &inputBox) {
    addTaskFromInput();
  } else if (&editor == editingEditor) {
    // Finish editing
    if (editingTaskIndex >= 0) {
      processor.editTask(editingTaskIndex, editingEditor->getText());
      editingEditor = nullptr;
      editingTaskIndex = -1;
    }
  }
}

void SimpleChecklistEditor::comboBoxChanged(juce::ComboBox *comboBox) {
  if (comboBox == &templateSelector) {
    int selected = templateSelector.getSelectedId();
    if (selected == 2)
      processor.loadTemplate("Mixing");
    else if (selected == 3)
      processor.loadTemplate("Mastering");
    else if (selected == 4)
      processor.loadTemplate("Recording");
    else if (selected == 5)
      processor.loadTemplate("Release");
    templateSelector.setSelectedId(1, juce::dontSendNotification);
  }
}

void SimpleChecklistEditor::mouseDoubleClick(const juce::MouseEvent &event) {
  int taskIndex = getTaskIndexAtPosition(event.y - taskViewport.getY() +
                                         taskViewport.getViewPositionY());
  if (taskIndex >= 0 &&
      taskIndex < static_cast<int>(processor.getTasks().size())) {
    // Start editing task
    editingTaskIndex = taskIndex;
    if (labels[taskIndex]) {
      auto *label = labels[taskIndex];
      editingEditor = new juce::TextEditor();
      editingEditor->setText(processor.getTasks()[taskIndex].text);
      editingEditor->setBounds(label->getBounds());
      editingEditor->setColour(juce::TextEditor::backgroundColourId,
                               juce::Colour(0xff3d3d3d));
      editingEditor->setColour(juce::TextEditor::textColourId,
                               juce::Colours::white);
      editingEditor->addListener(this);
      taskContainer.addAndMakeVisible(editingEditor);
      editingEditor->grabKeyboardFocus();
      label->setVisible(false);
    }
  }
}

void SimpleChecklistEditor::mouseDown(const juce::MouseEvent &event) {
  if (event.mods.isPopupMenu()) {
    int taskIndex = getTaskIndexAtPosition(event.y - taskViewport.getY() +
                                           taskViewport.getViewPositionY());
    if (taskIndex >= 0) {
      showContextMenu(taskIndex, event.getPosition());
    }
  }
}

void SimpleChecklistEditor::showContextMenu(int taskIndex, juce::Point<int>) {
  juce::PopupMenu menu;

  // Priority submenu
  juce::PopupMenu priorityMenu;
  priorityMenu.addItem(100, "None");
  priorityMenu.addItem(101, "Low");
  priorityMenu.addItem(102, "Medium");
  priorityMenu.addItem(103, "High");
  menu.addSubMenu("Set Priority", priorityMenu);

  // Category submenu
  juce::PopupMenu categoryMenu;
  categoryMenu.addItem(200, "General");
  categoryMenu.addItem(201, "Mix");
  categoryMenu.addItem(202, "Master");
  categoryMenu.addItem(203, "Record");
  categoryMenu.addItem(204, "Release");
  menu.addSubMenu("Set Category", categoryMenu);

  menu.addSeparator();
  menu.addItem(300, "Delete");

  menu.showMenuAsync(juce::PopupMenu::Options(), [this, taskIndex](int result) {
    if (result >= 100 && result < 104) {
      processor.setTaskPriority(taskIndex, static_cast<Priority>(result - 100));
    } else if (result >= 200 && result < 205) {
      processor.setTaskCategory(taskIndex, static_cast<Category>(result - 200));
    } else if (result == 300) {
      processor.removeTask(taskIndex);
    }
  });
}

int SimpleChecklistEditor::getTaskIndexAtPosition(int y) {
  int taskHeight = 35;
  return y / taskHeight;
}

void SimpleChecklistEditor::addTaskFromInput() {
  juce::String text = inputBox.getText().trim();
  if (text.isNotEmpty()) {
    processor.addTask(text);
    inputBox.clear();
  }
}

void SimpleChecklistEditor::tasksChanged() {
  rebuildTaskList();

  // Update progress
  int total = processor.getTotalCount();
  int completed = processor.getCompletedCount();
  progressLabel.setText(
      juce::String(completed) + " / " + juce::String(total) + " (" +
          juce::String(total > 0 ? (completed * 100 / total) : 0) + "%)",
      juce::dontSendNotification);
}

void SimpleChecklistEditor::rebuildTaskList() {
  checkboxes.clear();
  labels.clear();
  deleteButtons.clear();
  priorityLabels.clear();
  categoryLabels.clear();

  const auto &tasks = processor.getTasks();
  juce::String searchTerm = searchBox.getText().toLowerCase();
  int yPos = 0;

  for (int i = 0; i < static_cast<int>(tasks.size()); ++i) {
    const auto &task = tasks[i];

    // Filter by search
    if (searchTerm.isNotEmpty() &&
        !task.text.toLowerCase().contains(searchTerm)) {
      continue;
    }

    // Checkbox
    auto *checkbox = new juce::ToggleButton();
    checkbox->setToggleState(task.completed, juce::dontSendNotification);
    checkbox->addListener(this);
    checkbox->setBounds(10, yPos, 30, 30);
    taskContainer.addAndMakeVisible(checkbox);
    checkboxes.add(checkbox);

    // Priority indicator
    auto *priorityLabel = new juce::Label();
    priorityLabel->setText("●", juce::dontSendNotification);
    priorityLabel->setColour(juce::Label::textColourId,
                             getPriorityColour(task.priority));
    priorityLabel->setBounds(45, yPos, 15, 30);
    priorityLabel->setJustificationType(juce::Justification::centred);
    taskContainer.addAndMakeVisible(priorityLabel);
    priorityLabels.add(priorityLabel);

    // Category badge
    auto *categoryLabel = new juce::Label();
    categoryLabel->setText(getCategorySymbol(task.category),
                           juce::dontSendNotification);
    categoryLabel->setColour(juce::Label::textColourId,
                             juce::Colour(0xff888888));
    categoryLabel->setFont(juce::Font(10.0f));
    categoryLabel->setBounds(65, yPos, 30, 30);
    taskContainer.addAndMakeVisible(categoryLabel);
    categoryLabels.add(categoryLabel);

    // Task label
    auto *label = new juce::Label();
    label->setText(task.text, juce::dontSendNotification);
    label->setColour(juce::Label::textColourId, juce::Colours::white);
    label->setColour(juce::Label::backgroundColourId,
                     juce::Colours::transparentBlack);
    if (task.completed) {
      label->setColour(juce::Label::textColourId, juce::Colour(0xff666666));
    }
    label->setBounds(100, yPos, 280, 30);
    taskContainer.addAndMakeVisible(label);
    labels.add(label);

    // Delete button
    auto *delBtn = new juce::TextButton("X");
    delBtn->setColour(juce::TextButton::buttonColourId,
                      juce::Colour(0xff8b0000));
    delBtn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    delBtn->addListener(this);
    delBtn->setBounds(385, yPos, 40, 30);
    taskContainer.addAndMakeVisible(delBtn);
    deleteButtons.add(delBtn);

    yPos += 35;
  }

  taskContainer.setSize(430, yPos);
}

juce::Colour SimpleChecklistEditor::getPriorityColour(Priority priority) {
  switch (priority) {
  case Priority::High:
    return juce::Colour(0xffff4444);
  case Priority::Medium:
    return juce::Colour(0xffffaa00);
  case Priority::Low:
    return juce::Colour(0xff44ff44);
  default:
    return juce::Colour(0xffcccccc);
  }
}

juce::String SimpleChecklistEditor::getCategoryName(Category category) {
  switch (category) {
  case Category::Mix:
    return "Mix";
  case Category::Master:
    return "Master";
  case Category::Record:
    return "Record";
  case Category::Release:
    return "Release";
  default:
    return "General";
  }
}

juce::String SimpleChecklistEditor::getCategorySymbol(Category category) {
  switch (category) {
  case Category::Mix:
    return "[M]";
  case Category::Master:
    return "[MS]";
  case Category::Record:
    return "[R]";
  case Category::Release:
    return "[RL]";
  default:
    return "[G]";
  }
}
