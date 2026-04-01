import sys
import json
import os
from PIL import Image
from docx import Document
from docx.shared import Inches, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH

def format_time(time_val):
    """
    Formate le temps si le C++ envoie des millisecondes brutes, 
    ou renvoie le texte direct si le C++ a déjà formaté la chaîne.
    """
    if isinstance(time_val, str):
        return time_val
    
    seconds = time_val / 1000.0
    h = int(seconds // 3600)
    m = int((seconds % 3600) // 60)
    s = int(seconds % 60)
    f = int((seconds - int(seconds)) * 100)
    return f"{h:02d}:{m:02d}:{s:02d}:{f:02d}"

def main():
    # 1. Vérification des arguments
    if len(sys.argv) < 2:
        print("Erreur: Chemin du fichier JSON manquant.")
        sys.exit(1)

    json_path = sys.argv[1]
    
    # 2. Lecture du fichier JSON
    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    temp_dir = data['tempDir']
    dst_path = data['dstPath']
    
    # Assurer la bonne extension
    if not dst_path.endswith('.docx'):
        dst_path += '.docx'

    shots = data['shots']
    total_shots = len(shots)

    last_percent = -1

    try:
        doc = Document()
        
        titre_paragraphe = doc.add_paragraph()
        titre_paragraphe.alignment = WD_ALIGN_PARAGRAPH.CENTER
        
        titre_run = titre_paragraphe.add_run("Étude cinématographique")
        titre_run.font.bold = True
        titre_run.font.size = Pt(24)
        titre_run.font.color.rgb = RGBColor.from_string("EB4034")

        doc.add_paragraph()

        for idx, shot in enumerate(shots):
            time_str = format_time(shot.get('start', 0))
            end_str = format_time(shot.get('duration', 0))
            plan_title = shot.get('title', f"Plan {idx+1}")
            
            heading_text = f"- [Plan {idx+1}] {plan_title} -> Début : {time_str} / Durée : {end_str}"
            heading = doc.add_heading(heading_text, level=2)
            
            run_heading = heading.runs[0]
            run_heading.font.color.rgb = RGBColor.from_string("1F497D")

            img_path = os.path.join(temp_dir, shot['image'])
            if os.path.exists(img_path):
                try:
                    with Image.open(img_path) as img:
                        px_width, px_height = img.size

                    img_docx_width = Inches(px_width / 96.0)
                    if img_docx_width > Inches(6.0):
                        img_docx_width = Inches(6.0)

                    p_img = doc.add_paragraph()
                    p_img.alignment = WD_ALIGN_PARAGRAPH.CENTER

                    p_img.add_run().add_picture(img_path, width=img_docx_width)
                    
                except Exception as e:
                    print(f"Erreur lors du traitement de l'image {idx} : {e}", file=sys.stderr)

            note_text = shot.get('note', "").strip()
            if note_text:
                p_note = doc.add_paragraph(note_text)

            doc.add_paragraph()

            percent = int(((idx + 1) / total_shots) * 100)
            
            if percent != last_percent:
                print(f"PROGRESS:{percent}", flush=True)
                last_percent = percent 

        doc.save(dst_path)
        print("Fichier DOCX enregistré !", flush=True)

    except Exception as e:
        print(f"Erreur lors de l'exportation DOCX : {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()