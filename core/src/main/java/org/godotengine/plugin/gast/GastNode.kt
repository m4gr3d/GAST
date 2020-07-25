package org.godotengine.plugin.gast

import android.graphics.Canvas
import android.graphics.Color
import android.graphics.PorterDuff
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

    private var surfaceTexture: SurfaceTexture? = null
    private var surface: Surface? = null
    private var surfaceCanvas: Canvas? = null
    private var surfaceCanvasRefCount = 0

    var nodePath: String
        private set

    init {
        if (TextUtils.isEmpty(parentNodePath)) {
            throw IllegalArgumentException("Invalid parent node path value: $parentNodePath")
        }

        nodePath = acquireAndBindGastNode(parentNodePath, emptyParent)
        if (nodePath == RELEASED_PATH) {
            throw IllegalStateException("Unable to initialize node")
        }

        val texId = getTextureId()
        if (texId == INVALID_TEX_ID) {
            throw IllegalStateException("Unable to initialize node texture.")
        }

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

        unbindSurface()
        unbindAndReleaseGastNode(nodePath)

        nodePath = RELEASED_PATH
    }

    /**
     * Initialize and bind a [Surface] to this [GastNode] node.
     *
     * If the [Surface] is already bound, this method just returns it.
     */
    fun bindSurface(): Surface {
        if (surfaceTexture == null) {
            val texId = getTextureId()
            if (texId == INVALID_TEX_ID) {
                throw IllegalStateException("Unable to initialize node texture.")
            }

            surfaceTexture = SurfaceTexture(texId)
            surfaceTexture?.setOnFrameAvailableListener(this)
        }

        if (surface == null) {
            surface = Surface(surfaceTexture)
        }

        return surface!!
    }

    /**
     * Unbind the [Surface] bound to  this [GastNode] node.
     */
    fun unbindSurface() {
        if (surface != null) {
            surface?.release()
            surface = null
        }

        if (surfaceTexture != null) {
            surfaceTexture?.release()
            surfaceTexture = null
        }
    }

    fun isReleased() = nodePath == RELEASED_PATH

    private fun checkIfReleased() {
        if (isReleased()) {
            throw java.lang.IllegalStateException("GastNode is already released")
        }
    }

    /**
     * Update the surface texture size for this [GastNode] node.
     *
     * [bindSurface] must have been invoked at least once prior to invoking this method.
     * @throws IllegalStateException if a [Surface] is not bound to this [GastNode] node.
     */
    fun setSurfaceTextureSize(width: Int, height: Int) {
        surfaceTexture?.setDefaultBufferSize(width, height)
            ?: throw IllegalStateException("No Surface object bound to this node.")
    }

    /**
     * Gets a [Canvas] for drawing into the [Surface] bound to this node.
     *
     * After drawing into the provided [Canvas], the caller must invoke [unlockSurfaceCanvas] to
     * post the new contents.
     *
     * [bindSurface] must have been invoked at least once prior to invoking this method.
     * @throws IllegalStateException if a [Surface] is not bound to this [GastNode] node.
     */
    fun lockSurfaceCanvas(scaleX: Float = 1F, scaleY: Float = 1F): Canvas? {
        val boundSurface =
            surface ?: throw IllegalStateException("No Surface object bound to this node.")

        if (!boundSurface.isValid) {
            return null
        }

        if (surfaceCanvas == null) {
            if (surfaceCanvasRefCount != 0) {
                throw IllegalStateException("Invalid surface canvas state.")
            }

            surfaceCanvas = boundSurface.lockCanvas(null)
            surfaceCanvas?.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR)
            surfaceCanvas?.scale(scaleX, scaleY)
        }
        surfaceCanvasRefCount++
        return surfaceCanvas
    }

    /**
     * Post the new contents and release the [Canvas].
     *
     * [bindSurface] must have been invoked at least once prior to invoking this method.
     * @throws IllegalStateException if a [Surface] is not bound to this [GastNode] node.
     */
    fun unlockSurfaceCanvas() {
        val boundSurface =
            surface ?: throw IllegalStateException("No Surface object bound to this node.")

        if (surfaceCanvas == null || surfaceCanvasRefCount == 0) {
            throw IllegalStateException("Invalid surface canvas state.")
        }

        surfaceCanvasRefCount--
        if (surfaceCanvasRefCount == 0) {
            boundSurface.unlockCanvasAndPost(surfaceCanvas)
            surfaceCanvas = null
        }
    }

    private external fun acquireAndBindGastNode(
        parentNodePath: String,
        emptyParent: Boolean
    ): String

    private external fun unbindAndReleaseGastNode(nodePath: String)

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
            surfaceTexture?.updateTexImage()
            counter = updateTextureImageCounter.decrementAndGet()
        }
    }
}
