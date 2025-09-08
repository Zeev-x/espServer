Import("env")
import os
import urllib.request
import shutil

project_data_dir = os.path.join(env['PROJECT_DIR'], "data")
file_url = "https://raw.githubusercontent.com/Zeev-x/xrey/refs/heads/main/data/upload.jpg"
file_path = os.path.join(project_data_dir, "upload.jpg")

def before_upload(source, target, env):
    # Buat folder data jika belum ada
    if not os.path.exists(project_data_dir):
        os.makedirs(project_data_dir)
        print(">>> Folder data dibuat:", project_data_dir)

    # Unduh file jika belum ada
    if not os.path.isfile(file_path):
        print(">>> Mengunduh file upload.jpg dari GitHub...")
        urllib.request.urlretrieve(file_url, file_path)
        print(">>> File tersimpan di:", file_path)
    else:
        print(">>> File sudah ada, skip download.")

    # Jalankan upload filesystem
    print(">>> Upload filesystem...")
    env.Execute("pio run -t uploadfs -e {}".format(env['PIOENV']))

    # Hapus folder data setelah upload
    if os.path.exists(project_data_dir):
        shutil.rmtree(project_data_dir)
        print(">>> Folder data dihapus setelah upload.")

env.AddPreAction("upload", before_upload)
