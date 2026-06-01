import sys
import json
import os
from scenedetect import open_video, SceneManager, AdaptiveDetector, HashDetector

def segment_video(video_path, use_color=True):
    if not os.path.exists(video_path):
        print("ERROR: Fichier introuvable")
        return

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