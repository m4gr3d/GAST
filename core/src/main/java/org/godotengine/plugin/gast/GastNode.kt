package org.godotengine.plugin.gast

import android.graphics.Canvas
import android.graphics.Color
import android.graphics.PorterDuff
import android.graphics.SurfaceTexture
import android.os.Build
import android.text.TextUtils
import android.view.Surface
import androidx.annotation.RequiresApi
import org.godotengine.plugin.gast.projectionmesh.CustomProjectionMesh
import org.godotengine.plugin.gast.projectionmesh.EquirectangularProjectionMesh
import org.godotengine.plugin.gast.projectionmesh.ProjectionMesh
import org.godotengine.plugin.gast.projectionmesh.RectangularProjectionMesh
import java.util.BitSet
import java.util.concurrent.atomic.AtomicInteger

/**
 * @constructor Create a Gast node with the given parent node and set it up.
 * @property gastManager
 * @property parentNodePath - Path to the parent for the Gast node that will be created. The parent node must exist
 * @property emptyParent - If true, remove the children of the parent (if any) prior to inserting the Gast node
 */
class GastNode @JvmOverloads constructor(
    private val gastManager: GastManager,
    private var parentNodePath: String,
    emptyParent: Boolean = false
) : SurfaceTexture.OnFrameAvailableListener, GastRenderListener {

    private val updateTextureImageCounter = AtomicInteger()

    private var surfaceTexture: SurfaceTexture? = null
    private var surface: Surface? = null
    private var surfaceCanvas: Canvas? = null
    private var surfaceCanvasRefCount = 0

    private var nodePointer: Long
    val nodePath get() = nativeGetNodePath(nodePointer)

    private var projectionMeshPool : HashMap<Long, ProjectionMesh> = hashMapOf()

    // Mirrors enum ProjectionMeshType in src/main/cpp/gdn/gast_node.h
    enum class ProjectionMeshType {
        RECTANGULAR,
        EQUIRECTANGULAR,
        MESH,
    }

    init {
        if (TextUtils.isEmpty(parentNodePath)) {
            throw IllegalArgumentException("Invalid parent node path value: $parentNodePath")
        }

        nodePointer = acquireAndBindGastNode(parentNodePath, emptyParent)
        if (nodePointer == INVALID_NODE_POINTER) {
            throw IllegalStateException("Unable to initialize node")
        }

        val texId = getTextureId()
        if (texId == INVALID_TEX_ID) {
            throw IllegalStateException("Unable to initialize node texture.")
        }

        gastManager.registerGastRenderListener(this)
    }

    fun getProjectionMesh() : ProjectionMesh {
        checkIfReleased();
        val meshPointer = nativeGetProjectionMesh(nodePointer)

        if (projectionMeshPool.containsKey(meshPointer)) {
            return projectionMeshPool.getValue(meshPointer)
        }

        when(getProjectionMeshType()) {
            ProjectionMeshType.RECTANGULAR ->
                projectionMeshPool[meshPointer] =
                    RectangularProjectionMesh(meshPointer, nodePointer)
            ProjectionMeshType.EQUIRECTANGULAR ->
                projectionMeshPool[meshPointer] =
                    EquirectangularProjectionMesh(meshPointer, nodePointer)
            ProjectionMeshType.MESH -> {
                projectionMeshPool[meshPointer] = CustomProjectionMesh(meshPointer, nodePointer)
            }
        }

        return projectionMeshPool.getValue(meshPointer)
    }

    private external fun nativeGetProjectionMesh(nodePointer: Long) : Long

    fun getProjectionMeshType() : ProjectionMeshType {
        checkIfReleased()
        return ProjectionMeshType.values()[nativeGetProjectionMeshType(nodePointer)]
    }

    private external fun nativeGetProjectionMeshType(nodePointer: Long) : Int

    fun setProjectionMesh(projectionMeshType: ProjectionMeshType) {
        checkIfReleased()
        nativeSetProjectionMesh(nodePointer, projectionMeshType.ordinal)
    }

    private external fun nativeSetProjectionMesh(nodePointer: Long, projectionMeshType: Int)

    companion object {
        private val TAG = GastNode::class.java.simpleName
        private const val INVALID_TEX_ID = 0
        private const val INVALID_NODE_POINTER = 0L;
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
        unbindAndReleaseGastNode(nodePointer)
        nodePointer = INVALID_NODE_POINTER
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

    fun isReleased() = nodePointer == INVALID_NODE_POINTER

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
    fun lockSurfaceCanvas(): Canvas? {
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
            return
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
    ): Long

    private external fun unbindAndReleaseGastNode(nodePointer: Long)

    /**
     * Get the texture id for the Gast node with the given path.
     */
    fun getTextureId(): Int {
        checkIfReleased()
        return getTextureId(nodePointer)
    }

    private external fun getTextureId(nodePointer: Long): Int

    /**
     * Update the node's name.
     */
    fun setName(name: String) {
        checkIfReleased()
        nativeSetName(nodePointer, name)
    }

    private external fun nativeSetName(nodePointer: Long, name: String)

    /**
     * Reparent the given Gast node to the specified parent node.
     * @param newParentNodePath - Path to the new parent
     * @param emptyParent - If true, remove the children of the parent (if any) prior to inserting the Gast node
     */
    @JvmOverloads
    fun updateParent(newParentNodePath: String, emptyParent: Boolean = false) {
        checkIfReleased()
        if (parentNodePath != newParentNodePath) {
            if (TextUtils.isEmpty(newParentNodePath)) {
                throw IllegalArgumentException("Invalid parent node path value: $newParentNodePath")
            } else {
                if (updateGastNodeParent(nodePointer, newParentNodePath, emptyParent)) {
                    parentNodePath = newParentNodePath;
                }
            }
        }
    }

    private external fun updateGastNodeParent(
        nodePointer: Long,
        newParentNodePath: String,
        emptyParent: Boolean
    ): Boolean

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
        updateGastNodeVisibility(nodePointer, shouldDuplicateParentVisibility, visible)
    }

    private external fun updateGastNodeVisibility(
        nodePointer: Long,
        shouldDuplicateParentVisibility: Boolean,
        visible: Boolean
    )

    /**
     * Update the collision flag for the Gast node.
     * @param collidable - True to enable collision, false to disable.
     */
    fun setCollidable(collidable: Boolean) {
        checkIfReleased()
        setGastNodeCollidable(nodePointer, collidable)
    }

    private external fun setGastNodeCollidable(nodePointer: Long, collidable: Boolean)

    /**
     * Return true if collision is enabled for the node.
     */
    fun isCollidable(): Boolean {
        checkIfReleased()
        return isGastNodeCollidable(nodePointer)
    }

    private external fun isGastNodeCollidable(nodePointer: Long): Boolean

    fun isGazeTracking(): Boolean {
        checkIfReleased()
        return isGazeTracking(nodePointer)
    }

    private external fun isGazeTracking(nodePointer: Long): Boolean

    fun setGazeTracking(gaze_tracking: Boolean) {
        checkIfReleased()
        setGazeTracking(nodePointer, gaze_tracking)
    }

    private external fun setGazeTracking(nodePointer: Long, gaze_tracking: Boolean)

    fun isRenderOnTop(): Boolean {
        checkIfReleased()
        return isRenderOnTop(nodePointer)
    }

    private external fun isRenderOnTop(nodePointer: Long): Boolean

    fun setRenderOnTop(enable: Boolean) {
        checkIfReleased()
        setRenderOnTop(nodePointer, enable)
    }

    private external fun setRenderOnTop(nodePointer: Long, enable: Boolean)

    /**
     * Returns this node's collision layers.
     */
    @RequiresApi(Build.VERSION_CODES.KITKAT)
    fun getCollisionLayers(): BitSet {
        checkIfReleased()
        return BitSet.valueOf(longArrayOf(getCollisionLayers(nodePointer)))
    }

    private external fun getCollisionLayers(nodePointer: Long): Long

    /**
     * Update the node's collision layers.
     *
     * [Additional docs](https://docs.godotengine.org/en/stable/classes/class_physicsbody.html#class-physicsbody-property-collision-layer)
     */
    @RequiresApi(Build.VERSION_CODES.KITKAT)
    fun setCollisionLayers(layers: BitSet) {
        checkIfReleased()
        setCollisionLayers(nodePointer, layers.toLongArray()[0])
    }

    private external fun setCollisionLayers(nodePointer: Long, layers: Long)

    /**
     * Returns this node's collision masks.
     */
    @RequiresApi(Build.VERSION_CODES.KITKAT)
    fun getCollisionMasks(): BitSet {
        checkIfReleased()
        return BitSet.valueOf(longArrayOf(getCollisionMasks(nodePointer)))
    }

    private external fun getCollisionMasks(nodePointer: Long): Long

    /**
     * Update the node's collision masks.
     *
     * [Additional docs](https://docs.godotengine.org/en/stable/classes/class_physicsbody.html#class-physicsbody-property-collision-mask)
     */
    @RequiresApi(Build.VERSION_CODES.KITKAT)
    fun setCollisionMasks(masks: BitSet) {
        checkIfReleased()
        setCollisionMasks(nodePointer, masks.toLongArray()[0])
    }

    private external fun setCollisionMasks(nodePointer: Long, masks: Long)

    /**
     * Update the node's opacity.
     */
    fun updateAlpha(alpha: Float) {
        checkIfReleased()
        updateAlpha(nodePointer, alpha)
    }

    private external fun updateAlpha(nodePointer: Long, alpha: Float)

    /**
     * Translate the Gast node relative to its parent.
     */
    fun updateLocalTranslation(
        xTranslation: Float,
        yTranslation: Float,
        zTranslation: Float
    ) {
        checkIfReleased()
        updateGastNodeLocalTranslation(nodePointer, xTranslation, yTranslation, zTranslation)
    }

    private external fun updateGastNodeLocalTranslation(
        nodePointer: Long,
        xTranslation: Float,
        yTranslation: Float,
        zTranslation: Float
    )

    /**
     * Scale the Gast node relative to its parent.
     */
    fun updateLocalScale(xScale: Float, yScale: Float, zScale: Float) {
        checkIfReleased()
        updateGastNodeLocalScale(nodePointer, xScale, yScale, zScale)
    }

    private external fun updateGastNodeLocalScale(
        nodePointer: Long,
        xScale: Float,
        yScale: Float,
        zScale: Float
    )

    /**
     * Rotate the Gast node relative to its parent.
     */
    fun updateLocalRotation(
        xRotation: Float,
        yRotation: Float,
        zRotation: Float
    ) {
        checkIfReleased()
        updateGastNodeLocalRotation(nodePointer, xRotation, yRotation, zRotation)
    }

    private external fun updateGastNodeLocalRotation(
        nodePointer: Long,
        xRotation: Float,
        yRotation: Float,
        zRotation: Float
    )

    private external fun nativeGetNodePath(nodePointer: Long): String

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
