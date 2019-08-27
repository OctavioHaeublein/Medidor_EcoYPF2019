import time
import progressbar as pb

with pb.ProgressBar(max_value=1000) as bar:
    for i in range(100):
        time.sleep(0.1)
        bar.update(i)