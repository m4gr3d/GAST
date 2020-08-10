package org.godotengine.plugin.gast.video

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
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.extension.GastExtension
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

    override fun getPluginMethods() = mutableListOf(
        "preparePlayer",
        "play",
        "isPlaying",
        "pause",
        "seekTo",
        "stop",
        "setVideoScreenSize",
        "setRepeatMode"
    )

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

            Player.DISCONTINUITY_REASON_PERIOD_TRANSITION, Player.DISCONTINUITY_REASON_SEEK -> {
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
            val activity = activity?: return@runOnUiThread
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

    @Suppress("unused")
    fun preparePlayer(parentNodePath: String, videoRawNames: Array<String>) {
        setVideoNodePath(parentNodePath)
        setVideoSource(videoRawNames)
    }

    @Suppress("unused")
    fun setVideoScreenSize(width: Float, height: Float) {
        runOnRenderThread {
            gastNode?.updateSize(width, height)
        }
    }

    @Suppress("unused")
    fun play() {
        runOnUiThread {
            player.playWhenReady = true
        }

    }

    @Suppress("unused")
    fun isPlaying() = isInitialized() && playing.get()

    @Suppress("unused")
    fun pause() {
        runOnUiThread {
            player.playWhenReady = false
        }
    }

    @Suppress("unused")
    fun setRepeatMode(repeatMode: Int) {
        runOnUiThread {
            player.repeatMode = repeatMode
        }
    }

    @Suppress("unused")
    fun seekTo(positionInMsec: Int) {
        runOnUiThread {
            player.seekTo(positionInMsec.toLong())
        }
    }

    @Suppress("unused")
    fun stop() {
        runOnUiThread {
            player.stop()
        }
    }
}
