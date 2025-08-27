# Dockerfile located in /docs/demo/Dockerfile

FROM node:18-alpine

# Set working directory inside the container
WORKDIR /app

# Copy package.json and package-lock.json
COPY ../../package.json ../../package-lock.json ./

# Install dependencies
RUN npm install

# Copy full source code (assuming v86 source is in /docs/demo)
COPY ../../ ./

# Build the v86 project
RUN npm run build

# The build output (v86_all.js, v86.wasm etc.) usually lands in ./build or ./dist
# Copy or move the build output to a known folder, e.g. /docs/demo/build-output inside container

# For example, if output is in 'build/', move it to /app/build-output
RUN mkdir -p build-output && cp -r build/* build-output/

# Expose the build-output folder as volume or copy it out later
# By default Dockerfile ends here creating image with build artifacts in /app/build-output

CMD ["ls", "-l", "build-output"]
