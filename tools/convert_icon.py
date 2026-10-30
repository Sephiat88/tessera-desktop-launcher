"""Optional build-time SVG to PNG conversion: pip install cairosvg."""
import argparse
import cairosvg
parser = argparse.ArgumentParser()
parser.add_argument("source")
parser.add_argument("destination")
args = parser.parse_args()
cairosvg.svg2png(url=args.source, write_to=args.destination,
                output_width=128, output_height=128)
