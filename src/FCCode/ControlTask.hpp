#ifndef CONTROL_TASK_HPP_
#define CONTROL_TASK_HPP_

#include <memory>
#include <string>
#include <debug_console.hpp>
#include <Nameable.hpp>
#include <StateFieldBase.hpp>
#include <StateFieldRegistry.hpp>

/**
 * @brief A control ControlTaskBase is a wrapper around any high-level ControlTaskBase that
 * interacts with satellite data.
 *
 * This class is subclassed heavily in order to provide multiple
 * implementations of execute() for different kinds of ControlTasks.
 */
template <typename T>
class ControlTask : protected debug_console {
  public:
    /**
     * @brief Construct a new Control ControlTaskBase object
     *
     * @param name     Name of control ControlTaskBase
     * @param registry Pointer to state field registry
     */
    ControlTask(StateFieldRegistry& registry) : _registry(registry) {}

    /**
     * @brief Run main method of control ControlTaskBase.
     */
    virtual T execute() = 0;

    /**
     * @brief Destroy the Control Task object
     * 
     * We need to have this destructor to avoid compilation errors.
     */
    virtual ~ControlTask() = 0;

  protected:
    StateFieldRegistry& _registry;

    template<typename U>
    void add_internal_field(InternalStateField<U>& field) {
        const bool added = _registry.add_internal_field(
            static_cast<InternalStateFieldBase*>(&field));
        if(!added) {
            printf(debug_severity::error, "Field %s is already in the registry.",
                field.name().c_str());
            assert(false);
        }
    }

    template<typename U>
    void add_readable_field(ReadableStateField<U>& field) {
        const bool added = _registry.add_readable_field(
            static_cast<ReadableStateFieldBase*>(&field));
        if(!added) {
            printf(debug_severity::error, "Field %s is already in the registry.",
                field.name().c_str());
            assert(false);
        }
    }

    template<typename U>
    void add_writable_field(WritableStateField<U>& field) {
        const bool added = _registry.add_writable_field(
            static_cast<WritableStateFieldBase*>(&field));
        if(!added) {
            printf(debug_severity::error, "Field %s is already in the registry.",
                field.name().c_str());
            assert(false);
        }
    }

    template<typename U>
    InternalStateField<U>* find_internal_field(const char* field, const char* file, const unsigned int line) {
        auto field_ptr = _registry.find_internal_field(field);
        if (!field_ptr) {
            printf(debug_severity::error, "%s:%d: Internal field required is not present in state registry: %s\n", file, line, field);
            assert(false);
        }
        return static_cast<InternalStateField<U>*>(field_ptr);
    }

    template<typename U>
    ReadableStateField<U>* find_readable_field(const char* field, const char* file, const unsigned int line) {
        auto field_ptr = _registry.find_readable_field(field);
        if (!field_ptr) { 
            printf(debug_severity::error, "%s:%d: Readable field required is not present in state registry: %s\n", file, line, field);
            assert(false);
        }
        return static_cast<ReadableStateField<U>*>(field_ptr);
    }

    template<typename U>
    WritableStateField<U>* find_writable_field(const char* field, const char* file, const unsigned int line) {
        auto field_ptr = _registry.find_writable_field(field);
        if (!field_ptr) { 
            printf(debug_severity::error, "%s:%d: Writable field required is not present in state registry: %s\n", file, line, field);
            assert(false);
        }
        return static_cast<WritableStateField<U>*>(field_ptr);
    }
};

template<typename T>
ControlTask<T>::~ControlTask() {}

#endif