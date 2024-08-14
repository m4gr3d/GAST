package org.godotengine.plugin.gast.samples.spatialapp

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.google.android.exoplayer2.Player
import com.google.android.exoplayer2.SimpleExoPlayer
import com.google.android.exoplayer2.source.ConcatenatingMediaSource
import com.google.android.exoplayer2.source.ProgressiveMediaSource
import com.google.android.exoplayer2.upstream.DefaultDataSourceFactory
import com.google.android.exoplayer2.upstream.RawResourceDataSource
import com.google.android.exoplayer2.util.Util
import org.godotengine.plugin.gast.xr.view.XrSurfaceView
import org.godotengine.plugin.gast.xrapp.XrFragment

class VideoFragment: XrFragment(), Player.Listener {

    private val surfaceView: XrSurfaceView by lazy {
        requireView().findViewById(R.id.xr_video_surface)
    }

    private var player: SimpleExoPlayer? = null

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        bundle: Bundle?
    ): View? {
        return inflater.inflate(R.layout.video_fragment, container, false)
    }

    override fun onViewCreated(view: View, bundle: Bundle?) {
        super.onViewCreated(view, bundle)

        player = SimpleExoPlayer.Builder(requireContext()).build().apply {
            addListener(this@VideoFragment)
            setVideoSurfaceHolder(surfaceView.getHolder())
        }
    }

    override fun onResume() {
        super.onResume()

        player?.apply {
            val dataSourceFactory = DefaultDataSourceFactory(requireContext(), Util.getUserAgent(requireContext(), "TestFragment"))
            val videosSource = ConcatenatingMediaSource()
            val videoUri = RawResourceDataSource.buildRawResourceUri(R.raw.flight)
            val source = ProgressiveMediaSource.Factory(dataSourceFactory).setTag(R.raw.flight).createMediaSource(videoUri)
            videosSource.addMediaSource(source)

            prepare(videosSource)

            repeatMode = Player.REPEAT_MODE_ONE
            playWhenReady = true
        }
    }

    override fun onPause() {
        super.onPause()

        player?.apply {
            playWhenReady = false
            stop()
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()

        player?.release()
    }
}
