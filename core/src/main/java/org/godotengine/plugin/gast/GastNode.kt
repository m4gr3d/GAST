package org.godotengine.plugin.gast

import android.graphics.SurfaceTexture
import android.text.TextUtils
import android.view.Surface
import java.util.concurrent.atomic.AtomicInteger

/**
 * @constructor Create a Gast node with the given parent node and set it up.
 * @property gastManager
 * @property parentNodePath - Path to the parent for the Gast node that will be created. The parent node must exist
 * @property emptyParent - If true, remove the children of the parent (if any) prior to inserting the Gast node
 */
class GastNode @JvmOverloads constructor(
    private val gastManager: GastManager,
    private val parentNodePath: String,
    emptyParent: Boolean = false
) : SurfaceTexture.OnFrameAvailableListener, GastRenderListener {

    private val updateTextureImageCounter = AtomicInteger()
    val surfaceTexture: SurfaceTexture
    val surface: Surface
    var nodePath: String
        private set

    init {
        if (TextUtils.isEmpty(parentNodePath)) {
            throw IllegalArgumentException("Invalid parent node path value: $parentNodePath")
        }

        nodePath = gastManager.acquireAndBindGastNode(parentNodePath, emptyParent)
        if (nodePath == RELEASED_PATH) {
            throw IllegalStateException("Unable to initialize node")
        }

        val texId = getTextureId()
        if (texId == INVALID_TEX_ID) {
            throw IllegalStateException("Unable to initialize node texture.")
        }

        surfaceTexture = SurfaceTexture(texId)
        surfaceTexture.setOnFrameAvailableListener(this)
        surface = Surface(surfaceTexture)

        gastManager.registerGastRenderListener(this)
    }

    companion object {
        private val TAG = GastNode::class.java.simpleName
        private const val INVALID_SURFACE_INDEX = -1
        private const val INVALID_TEX_ID = 0
        private const val RELEASED_PATH = ""
    }

    /**
     * Release the [GastNode].
     *
     * The node is no longer usable after this method is invoked.
     */
    fun release() {
        if (isReleased()) {
            return
        }

        gastManager.unregisterGastRenderListener(this)

        surface.release()
        surfaceTexture.release()
        gastManager.unbindAndReleaseGastNode(nodePath)

        nodePath = RELEASED_PATH
    }

    fun isReleased() = nodePath == RELEASED_PATH

    private fun checkIfReleased() {
        if (isReleased()) {
            throw java.lang.IllegalStateException("GastNode is already released")
        }
    }

    /**
     * Get the texture id for the Gast node with the given path.
     */
    @JvmOverloads
    fun getTextureId(surfaceIndex: Int = INVALID_SURFACE_INDEX): Int {
        checkIfReleased()
        return getTextureId(nodePath, surfaceIndex)
    }

    private external fun getTextureId(
        nodePath: String,
        surfaceIndex: Int
    ): Int

    /**
     * Reparent the given Gast node to the specified parent node.
     * @param newParentNodePath - Path to the new parent
     * @param emptyParent - If true, remove the children of the parent (if any) prior to inserting the Gast node
     */
    @JvmOverloads
    fun updateParent(newParentNodePath: String, emptyParent: Boolean = false) {
        checkIfReleased()
        if (parentNodePath != newParentNodePath) {
            nodePath = updateGastNodeParent(nodePath, newParentNodePath, emptyParent)
        }
    }

    private external fun updateGastNodeParent(
        nodePath: String,
        newParentNodePath: String,
        emptyParent: Boolean
    ): String

    /**
     * Update the visibility of the given Gast node.
     * @param shouldDuplicateParentVisibility - Whether the node should match its parent's visibility
     * @param visible - True to make the node visible, false otherwise
     */
    fun updateVisibility(
        shouldDuplicateParentVisibility: Boolean,
        visible: Boolean
    ) {
        checkIfReleased()
        updateGastNodeVisibility(nodePath, shouldDuplicateParentVisibility, visible)
    }

    private external fun updateGastNodeVisibility(
        nodePath: String,
        shouldDuplicateParentVisibility: Boolean,
        visible: Boolean
    )

    /**
     * Update the collision flag for the Gast node.
     * @param collidable - True to enable collision, false to disable.
     */
    fun setCollidable(collidable: Boolean) {
        checkIfReleased()
        setGastNodeCollidable(nodePath, collidable)
    }

    private external fun setGastNodeCollidable(nodePath: String, collidable: Boolean)

    /**
     * Return true if collision is enabled for the node.
     */
    fun isCollidable(): Boolean {
        checkIfReleased()
        return isGastNodeCollidable(nodePath)
    }

    private external fun isGastNodeCollidable(nodePath: String): Boolean

    /**
     * Update the size of the Gast node.
     */
    fun updateSize(width: Float, height: Float) {
        checkIfReleased()
        updateGastNodeSize(nodePath, width, height)
    }

    private external fun updateGastNodeSize(nodePath: String, width: Float, height: Float)

    /**
     * Translate the Gast node relative to its parent.
     */
    fun updateLocalTranslation(
        xTranslation: Float,
        yTranslation: Float,
        zTranslation: Float
    ) {
        checkIfReleased()
        updateGastNodeLocalTranslation(nodePath, xTranslation, yTranslation, zTranslation)
    }

    private external fun updateGastNodeLocalTranslation(
        nodePath: String,
        xTranslation: Float,
        yTranslation: Float,
        zTranslation: Float
    )

    /**
     * Scale the Gast node relative to its parent.
     */
    fun updateLocalScale(xScale: Float, yScale: Float) {
        checkIfReleased()
        updateGastNodeLocalScale(nodePath, xScale, yScale)
    }

    private external fun updateGastNodeLocalScale(nodePath: String, xScale: Float, yScale: Float)

    /**
     * Rotate the Gast node relative to its parent.
     */
    fun updateLocalRotation(
        xRotation: Float,
        yRotation: Float,
        zRotation: Float
    ) {
        checkIfReleased()
        updateGastNodeLocalRotation(nodePath, xRotation, yRotation, zRotation)
    }

    private external fun updateGastNodeLocalRotation(
        nodePath: String,
        xRotation: Float,
        yRotation: Float,
        zRotation: Float
    )

    override fun onFrameAvailable(surfaceTexture: SurfaceTexture) {
        updateTextureImageCounter.incrementAndGet()
    }

    override fun onRenderDrawFrame() {
        var counter = updateTextureImageCounter.get()
        while (counter > 0) {
            surfaceTexture.updateTexImage()
            counter = updateTextureImageCounter.decrementAndGet()
        }
    }
}
