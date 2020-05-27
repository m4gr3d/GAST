package org.godotengine.plugin.gast

/**
 * Used to listen to render events dispatched by the Gast plugin.
 */
interface GastRenderListener {
    /**
     * Forward the render draw frame callback.
     *
     * This is invoked on the render thread.
     */
    fun onRenderDrawFrame()
}
