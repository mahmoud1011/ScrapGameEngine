#pragma once
#include <functional>
#include <map>

/**
 * @class Signal
 * @brief A signal object that can call multiple slots with the same signature.
 *
 * A signal object allows you to connect functions to it which will be called when the `emit()` method is invoked.
 * Any argument passed to `emit()` will be forwarded to the connected functions.
 *
 * @tparam Args The argument types that the connected functions expect.
 */
template <typename... Args>
class Signal {
public:
	/**
     * @brief Default constructor.
     */
	Signal() = default;

	/**
	 * @brief Destructor.
	 */
	~Signal() = default;

	/**
	 * @brief Copy constructor.
	 *
	 * This constructor creates a new signal.
	 *
	 * @param unused Unused parameter.
	 */
	Signal(Signal const& /*unused*/) {}

	/**
	 * @brief Copy assignment operator.
	 *
	 * @param other The other `Signal` object to copy from.
	 * @return A reference to this signal.
	 */
	Signal& operator=(Signal const& other) {
		if (this != &other) {
			disconnect_all();
		}
		return *this;
	}

	/**
	 * @brief Move constructor.
	 *
	 * Transfers ownership of slots from the other signal to this one.
	 *
	 * @param other The other `Signal` object to move from.
	 */
	Signal(Signal&& other) noexcept :
		_slots(std::move(other._slots)),
		_current_id(other._current_id) {}

	/**
	 * @brief Move assignment operator.
	 *
	 * @param other The other `Signal` object to move from.
	 * @return A reference to this signal.
	 */
	Signal& operator=(Signal&& other) noexcept {
		if (this != &other) {
			_slots = std::move(other._slots);
			_current_id = other._current_id;
		}

		return *this;
	}

	/**
	 * @brief Connects a `std::function` to the signal.
	 *
	 * The connected function will be called when `emit()` is invoked.
	 *
	 * @param slot The function to connect.
	 * @return An ID that can be used to disconnect the function.
	 */
	int connect(std::function<void(Args...)> const& slot) const {
		_slots.insert(std::make_pair(++_current_id, slot));
		return _current_id;
	}

	/**
	 * @brief Convenience method to connect a member function of an object to this signal.
	 *
	 * @tparam T The type of the object.
	 * @param inst A pointer to the object instance.
	 * @param func A member function pointer.
	 * @return An ID that can be used to disconnect the function.
	 */
	template <typename T>
	int connect_member(T* inst, void (T::* func)(Args...)) {
		return connect([=](Args... args) {
			(inst->*func)(args...);
			});
	}

	/**
	 * @brief Convenience method to connect a const member function of an object to this signal.
	 *
	 * @tparam T The type of the object.
	 * @param inst A pointer to the object instance.
	 * @param func A const member function pointer.
	 * @return An ID that can be used to disconnect the function.
	 */
	template <typename T>
	int connect_member(T* inst, void (T::* func)(Args...) const) {
		return connect([=](Args... args) {
			(inst->*func)(args...);
			});
	}

	/**
	 * @brief Disconnects a previously connected function.
	 *
	 * @param id The ID of the connection to disconnect.
	 */
	void disconnect(int id) const {
		_slots.erase(id);
	}

	/**
	 * @brief Disconnects all previously connected functions.
	 */
	void disconnect_all() const {
		_slots.clear();
	}

	/**
	 * @brief Calls all connected functions.
	 *
	 * @param p Arguments to pass to the connected functions.
	 */
	void emit(Args... p) {
		for (auto const& it : _slots) {
			it.second(p...);
		}
	}

	/**
	 * @brief Calls all connected functions except for one.
	 *
	 * @param excludedConnectionID The ID of the function to exclude.
	 * @param p Arguments to pass to the connected functions.
	 */
	void emit_for_all_but_one(int excludedConnectionID, Args... p) {
		for (auto const& it : _slots) {
			if (it.first != excludedConnectionID) {
				it.second(p...);
			}
		}
	}

	/**
	 * @brief Calls only one connected function.
	 *
	 * @param connectionID The ID of the function to call.
	 * @param p Arguments to pass to the connected function.
	 */
	void emit_for(int connectionID, Args... p) {
		auto const& it = _slots.find(connectionID);
		if (it != _slots.end()) {
			it->second(p...);
		}
	}

private:
	mutable std::map<int, std::function<void(Args...)>> _slots; ///< A map of connections.
	mutable int                                         _current_id{ 0 }; ///< The current connection ID.
};