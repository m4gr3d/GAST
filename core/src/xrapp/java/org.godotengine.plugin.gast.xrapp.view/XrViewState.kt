package org.godotengine.plugin.gast.xrapp.view

import android.view.View
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.projectionmesh.RectangularProjectionMesh

/**
 * Keep track of the state of a [XrView] instance.
 */
class XrViewState(val view: View) {
    internal val inputManager = XrViewInputManager(this)
    internal val renderManager = XrViewRenderManager(this)
    internal val children: MutableSet<XrView> = HashSet()

    internal var parent: XrView? = null
    internal var gastNode: GastNode? = null
    internal var gastManager: GastManager? = null
    internal var initialized: Boolean = false
        private set

    // View properties
    internal var gazeTracking = false
        set(value) {
            field = value
            gastNode?.setGazeTracking(value)
        }

    internal var curved = false
        set(value) {
            field = value
            val projectionMesh = gastNode?.getProjectionMesh()
            if (projectionMesh is RectangularProjectionMesh) {
                projectionMesh.setCurved(value)
            }
        }

    fun initialize(gastManager: GastManager, gastNode: GastNode) {
        if (initialized) {
            return
        }

        this.gastManager = gastManager
        this.gastNode = gastNode
        this.parent = null

        // Propagate view properties current state
        gastNode.setGazeTracking(gazeTracking)

        val projectionMesh = gastNode.getProjectionMesh()
        if (projectionMesh is RectangularProjectionMesh) {
            projectionMesh.setCurved(curved)
        }

        initialized = true
    }

    fun shutdown() {
        if (!initialized) {
            return
        }

        gastNode?.release()
        gastNode = null

        gastManager = null
        parent = null

        initialized = false
    }
}
