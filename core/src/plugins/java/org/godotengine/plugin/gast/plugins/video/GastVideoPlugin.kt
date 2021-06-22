package org.godotengine.plugin.gast.plugins.video

import android.text.TextUtils
import android.util.Log
import com.google.android.exoplayer2.ExoPlaybackException
import com.google.android.exoplayer2.Player
import com.google.android.exoplayer2.SimpleExoPlayer
import com.google.android.exoplayer2.source.ConcatenatingMediaSource
import com.google.android.exoplayer2.source.ProgressiveMediaSource
import com.google.android.exoplayer2.upstream.DefaultDataSourceFactory
import com.google.android.exoplayer2.upstream.RawResourceDataSource
import com.google.android.exoplayer2.util.Util
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.UsedByGodot
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.extension.GastExtension
import org.godotengine.plugin.gast.projectionmesh.RectangularProjectionMesh
import java.util.concurrent.atomic.AtomicBoolean

/**
 * GAST-Video plugin
 *
 * This plugin is used to play audio/video contents in the Godot engine.
 * It's powered by Exoplayer and thus support a wide range of codecs.
 */
class GastVideoPlugin(godot: Godot) : GastExtension(godot), Player.EventListener {

    companion object {
        private val TAG = GastVideoPlugin::class.java.simpleName
    }

    private val player = SimpleExoPlayer.Builder(activity!!).build()
    private val playing = AtomicBoolean(false)
    private var gastNode: GastNode? = null

    init {
        player.addListener(this)
    }

    override fun getPluginName() = "gast-video"

    override fun onMainDestroy() {
        super.onMainDestroy()
        player.release()
        gastNode?.release()
    }

    override fun onIsPlayingChanged(isPlaying: Boolean) {
        playing.set(isPlaying)
    }

    override fun onPlayerError(error: ExoPlaybackException) {
        Log.e(TAG, error.message, error)
    }

    override fun onPositionDiscontinuity(@Player.DiscontinuityReason reason: Int) {
        when (reason) {

            Player.DISCONTINUITY_REASON_AUTO_TRANSITION, Player.DISCONTINUITY_REASON_SEEK -> {
                // TODO: use to detect when switching video in a playlist
            }
            else -> {
            }
        }
    }

    private fun isInitialized() = gastNode != null && !gastNode!!.isReleased()

    private fun setVideoNodePath(parentNodePath: String) {
        if (TextUtils.isEmpty(parentNodePath)) {
            Log.e(TAG, "Invalid parent node path value: $parentNodePath")
            return
        }

        runOnRenderThread {
            if (isInitialized()) {
                gastNode?.updateParent(parentNodePath)
            } else {
                gastNode = GastNode(gastManager, parentNodePath)
            }

            runOnUiThread {
                player.setVideoSurface(gastNode?.bindSurface())
            }
        }
    }

    private fun setVideoSource(videoRawNames: Array<String>) {
        if (videoRawNames.isEmpty()) {
            Log.e(TAG, "No available videos to load: $videoRawNames")
            return
        }

        runOnUiThread {
            val activity = activity ?: return@runOnUiThread
            val resources = activity.resources
            val packageName = activity.packageName
            val dataSourceFactory =
                DefaultDataSourceFactory(activity, Util.getUserAgent(activity, pluginName))
            val videosSource = ConcatenatingMediaSource()

            for (videoRawName in videoRawNames) {
                val videoRawId = resources.getIdentifier(videoRawName, "raw", packageName)
                if (videoRawId == 0) {
                    Log.e(
                        TAG,
                        "Unable to find video $videoRawName in the resources 'raw' directory."
                    )
                    continue
                }

                val uri = RawResourceDataSource.buildRawResourceUri(videoRawId)
                val source =
                    ProgressiveMediaSource.Factory(dataSourceFactory)
                        .setTag(videoRawId)
                        .createMediaSource(uri)
                videosSource.addMediaSource(source)
            }
            player.prepare(videosSource)
        }
    }

    /**
     * Setup the player to use for media playback. Invoking this method is required prior to using
     * the player for media playback.
     *
     * @param[parentNodePath] Node path for the node that should contain the player.
     * @param[videoRawNames] Names of the media files used for playback.
     * For now, the media files should be lower case with no spaces, be located in the project's
     * raw resources directory and the file extension should be omitted (e.g: `res/raw/flight.mp4` -> ["flight"]).
     */
    @Suppress("unused")
    @UsedByGodot
    fun preparePlayer(parentNodePath: String, videoRawNames: Array<String>) {
        setVideoNodePath(parentNodePath)
        setVideoSource(videoRawNames)
    }

    /**
     * Enable / disable collision with the player screen.
     *
     * @param[collidable] True to enable collision, false otherwise.
     */
    @Suppress("unused")
    @UsedByGodot
    fun setVideoScreenCollidable(collidable: Boolean) {
        runOnRenderThread {
            gastNode?.setCollidable(collidable)
        }
    }

    /**
     * Enable / disable curving of the player screen.
     *
     * @param[curved] True to curve the player screen, false otherwise.
     */
    @Suppress("unused")
    @UsedByGodot
    fun setVideoScreenCurved(curved: Boolean) {
        runOnRenderThread {
            if (gastNode?.getProjectionMeshType() == GastNode.ProjectionMeshType.RECTANGULAR) {
                val projectionMesh: RectangularProjectionMesh =
                    gastNode?.getProjectionMesh() as RectangularProjectionMesh
                projectionMesh.setCurved(curved)
            }
        }
    }

    /**
     * Update the player screen size.
     *
     * @param[width] Width of the player screen.
     * @param[height] Height of the player screen.
     */
    @Suppress("unused")
    @UsedByGodot
    fun setVideoScreenSize(width: Float, height: Float) {
        runOnRenderThread {
            if (gastNode?.getProjectionMeshType() == GastNode.ProjectionMeshType.RECTANGULAR) {
                val projectionMesh: RectangularProjectionMesh =
                    gastNode?.getProjectionMesh() as RectangularProjectionMesh
                projectionMesh.setMeshSize(width, height)
            }
        }
    }

    /**
     * Resume media playback if paused or stopped.
     */
    @Suppress("unused")
    @UsedByGodot
    fun play() {
        runOnUiThread {
            player.playWhenReady = true
        }

    }

    /**
     * @return True if the player is setup and media is playing, false otherwise.
     */
    @Suppress("unused")
    @UsedByGodot
    fun isPlaying() = isInitialized() && playing.get()

    /**
     * Pause media playback.
     */
    @Suppress("unused")
    @UsedByGodot
    fun pause() {
        runOnUiThread {
            player.playWhenReady = false
        }
    }

    /**
     * Repeat mode for media playback.
     *
     * @param[repeatMode] Should be one of:
     * - REPEAT_MODE_OFF = 0; Normal playback without repetition.
     * - REPEAT_MODE_ONE = 1; "Repeat One" mode to repeat the currently playing media infinitely.
     * - REPEAT_MODE_ALL = 2; "Repeat All" mode to repeat the entire timeline infinitely.
     */
    @Suppress("unused")
    @UsedByGodot
    fun setRepeatMode(@Player.RepeatMode repeatMode: Int) {
        runOnUiThread {
            player.repeatMode = repeatMode
        }
    }

    /**
     * Seeks to a position specified in milliseconds.
     *
     * @param[positionInMsec] The seek position to target.
     */
    @Suppress("unused")
    @UsedByGodot
    fun seekTo(positionInMsec: Int) {
        runOnUiThread {
            player.seekTo(positionInMsec.toLong())
        }
    }

    /**
     * Stop playback.
     */
    @Suppress("unused")
    @UsedByGodot
    fun stop() {
        runOnUiThread {
            player.stop()
        }
    }
}
