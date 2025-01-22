FROM devkitpro/devkitarm:20250102

RUN apt-get update -y && apt-get install --no-install-recommends -y \
	python3 \
	python3-pycryptodome \
&& rm -rf /var/lib/apt/lists/*

WORKDIR /app
CMD make -j$(nproc)

# number of wasted CI runs: 1
