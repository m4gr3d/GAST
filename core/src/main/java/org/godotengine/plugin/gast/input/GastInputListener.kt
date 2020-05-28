package org.godotengine.plugin.gast.input

/**
 * Used to listen to input events dispatched by the Gast plugin.
 */
interface GastInputListener {
    /**
     * Callback for hover input events.
     *
     * This is invoked on the main thread.
     */
    fun onMainInputHover(nodePath: String, pointerId: String, xPercent: Float, yPercent: Float)

    /**
     * Callback for press input events.
     *
     * This is invoked on the main thread.
     */
    fun onMainInputPress(nodePath: String, pointerId: String, xPercent: Float, yPercent: Float)

    /**
     * Callback for release input events.
     *
     * This is invoked on the main thread.
     */
    fun onMainInputRelease(nodePath: String, pointerId: String, xPercent: Float, yPercent: Float)

    /**
     * Callback for scroll input events.
     *
     * This is invoked on the main thread.
     */
    fun onMainInputScroll(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float,
        horizontalDelta: Float,
        verticalDelta: Float
    )
}
