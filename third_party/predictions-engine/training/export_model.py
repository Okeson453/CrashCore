"""Export logistic weights to models/validation/model.bin (PEML v1)."""
import struct
import hashlib
from pathlib import Path
from feature_export import ML_FEATURE_ORDER

MAGIC = 0x50454D4C
VERSION = 1

def write_model(path: Path, bias: float, weights: list[float],
                mean=None, scale=None, platt_a=1.0, platt_b=0.0):
    n = len(weights)
    buf = bytearray()
    buf += struct.pack("<III", MAGIC, VERSION, n)
    buf += struct.pack("<d", bias)
    for w in weights:
        buf += struct.pack("<d", w)
    if mean is not None and scale is not None:
        buf += struct.pack("<B", 1)
        for v in mean: buf += struct.pack("<d", v)
        for v in scale: buf += struct.pack("<d", v)
    else:
        buf += struct.pack("<B", 0)
    buf += struct.pack("<B", 1)  # Platt
    buf += struct.pack("<dd", platt_a, platt_b)
    path.write_bytes(buf)
    checksum = hashlib.sha256(buf).hexdigest()
    path.with_suffix(".sha256").write_text(checksum + "\n")
    return checksum

if __name__ == "__main__":
    out = Path(__file__).resolve().parents[1] / "models" / "validation"
    out.mkdir(parents=True, exist_ok=True)
    n = len(ML_FEATURE_ORDER)
    weights = [0.0] * n
    for i, k in enumerate(ML_FEATURE_ORDER):
        if k == "ewma_hit_13": weights[i] = 1.2
        elif k == "markov_p_up": weights[i] = 0.4
        elif k == "short_hit_13": weights[i] = 0.5
        elif k == "entropy_binary_13": weights[i] = -0.2
    import math
    bias = math.log(0.65 / 0.35)
    cs = write_model(out / "model.bin", bias, weights)
    print(f"wrote {out / 'model.bin'} checksum={cs}")
