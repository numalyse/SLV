import sys
import json
import os
import cv2
from scenedetect import open_video, SceneManager, AdaptiveDetector, HashDetector


def is_video_grayscale(video_path, samples=10, sat_threshold=15.0, grayscale_ratio=0.80):
    """
    Detect if a video is in grayscale (black and white).

    Args:
        video_path (str): Path to the video file.
        samples (int): Number of frames to sample.
        sat_threshold (float): Mean HSV saturation (0-255) below which a frame is considered grayscale.
        grayscale_ratio (float): Minimum ratio of grayscale frames to consider the video as grayscale.

    Returns:
        bool: True if the video is grayscale, False otherwise.
    """

    cap = cv2.VideoCapture(video_path)
    if not cap.isOpened():
        return False

    try:
        total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
        if total_frames <= 0:
            return False

        grayscale_count = 0
        measured = 0

        # sample frames at regular intervals and measure their saturation
        for i in range(samples):
            cap.set(cv2.CAP_PROP_POS_FRAMES, int(total_frames * (i + 0.5) / samples))
            ok, frame = cap.read()
            if not ok or frame is None:
                continue

            # downscale large frames
            height, width = frame.shape[:2]
            if width > 320:
                frame = cv2.resize(frame, (320, max(1, int(height * 320 / width))))

            hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
            saturation = hsv[:, :, 1].mean() # mean of canal 1 which is saturation

            measured += 1
            if saturation <= sat_threshold:
                grayscale_count += 1

        if measured == 0:
            return False

        return (grayscale_count / measured) >= grayscale_ratio
    finally:
        cap.release()


def segment_video(video_path, use_color=True):
    if not os.path.exists(video_path):
        print("[SegmentationPy] Fichier media introuvable", video_path)
        return

    if use_color and is_video_grayscale(video_path):
        use_color = False
        print("[SegmentationPy] Film detecte en noir et blanc", flush=True)

    video = open_video(video_path)

    total_frames = video.duration.frame_num
    if total_frames <= 0:
        total_frames = 1

    scene_manager = SceneManager()

    if use_color:
        scene_manager.add_detector(AdaptiveDetector())
    else:
        scene_manager.add_detector(HashDetector())

    def progress_callback(frame_img, frame_timecode):
        frame_num = frame_timecode.frame_num
        percent = int((frame_num / total_frames) * 100)
        percent = min(percent, 100)
        print(f"PROGRESS:{percent}", flush=True)

    scene_manager.detect_scenes(video, callback=progress_callback)
    scene_list = scene_manager.get_scene_list()

    cuts = [
        scene[0].frame_num
        for scene in scene_list
        if scene[0].frame_num > 0
    ]

    print(f"RESULT:{json.dumps(cuts)}", flush=True)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.exit(1)

    path = sys.argv[1]
    color_mode = True if len(sys.argv) > 2 and sys.argv[2] == "1" else False

    segment_video(path, color_mode)
