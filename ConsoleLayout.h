#pragma once

#include <string>

class ConsoleSystem;

/**
 * @class ConsoleLayout
 * @brief Abstract base class representing a layout in a console application.
 *
 * This class provides an interface for different console layouts, allowing
 * derived classes to implement custom rendering and input processing logic.
 *
 * @protected
 *   - name: The name of the layout.
 *
 * @public
 *   - ConsoleLayout(const std::string& layoutName): Constructor that initializes the layout with a given name.
 *   - virtual ~ConsoleLayout(): Virtual destructor for proper cleanup in derived classes.
 *   - virtual void render() = 0: Pure virtual function to render the layout. Must be implemented by derived classes.
 *   - virtual bool processInput(const std::string input) = 0: Pure virtual function to process user input. Must be implemented by derived classes.
 *   - std::string getName() const: Returns the name of the layout.
 */
class ConsoleLayout {

protected:
    std::string name;

public:
    ConsoleLayout(const std::string& layoutName)
        : name(layoutName) {
    }

    virtual ~ConsoleLayout() = default;

    virtual void render() = 0;

    virtual bool processInput(const std::string input) = 0;

    std::string getName() const { return name; }
};