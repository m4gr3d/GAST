package org.godotengine.plugin.gast.input.action

/**
 * Used to monitor for input action events.
 */
interface GastActionListener {

    /**
     * Classifies the various input press state.
     */
    enum class InputPressState(private val index: Int) {
        /**
         * Invalid state.
         */
        INVALID(-1),
        /**
         * Active when the user starts pressing the action event, meaning it's active only on the
         * frame that the action event was started.
         */
        JUST_PRESSED(0),

        /**
         * Active when the user is pressing the action event.
         */
        PRESSED(1),

        /**
         * Active when the user stops pressing the action event, meaning it's active only on the
         * frame that the action event was released.
         */
        JUST_RELEASED(2);

        companion object {
            internal fun fromIndex(index: Int): InputPressState = when(index) {
                JUST_PRESSED.index -> JUST_PRESSED
                PRESSED.index -> PRESSED
                JUST_RELEASED.index -> JUST_RELEASED
                else -> INVALID
            }
        }
    }

    /**
     * Return a set of input actions to monitor.
     * @see [onMainInputAction]
     * @see https://docs.godotengine.org/en/stable/classes/class_inputeventaction.html
     */
    fun getInputActionsToMonitor(): Set<String> = emptySet()

    /**
     * Callback for input action events.
     *
     * This is invoked on the main thread.
     * @see [getInputActionsToMonitor]
     * @see https://docs.godotengine.org/en/stable/classes/class_inputeventaction.html
     */
    fun onMainInputAction(action: String, pressState: InputPressState, strength: Float)
}
