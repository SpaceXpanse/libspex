# Builds a Docker image that contains the SpaceXpanse ships binaries as well as
# Charon binaries for running the SpaceXpanseships GSP client/server for
# light mode.

# Note:  This image is based on spacexpanse/charon, which itself is based on
# spacexpanse/libspex, not on the libspex image directly!

FROM spacexpanse/charon AS build

# Collect the binaries and required libraries.  Also add in the Charon
# binaries.
WORKDIR /jail
RUN mkdir bin && cp /usr/local/bin/ships* /usr/local/bin/charon-* bin/
RUN for b in bin/*; do cpld $b lib64; done

# Add extra files needed (like the channel GSP method spec loaded with the
# Charon binaries).
RUN mkdir share \
  && cp /usr/local/share/channel-gsp-rpc.json share/ \
  && cp /usr/local/share/charon/letsencrypt.pem share/
COPY ships/docker/entrypoint.sh bin/

# Build the final image.
FROM alpine
COPY --from=build /jail /usr/local/
ENV LD_LIBRARY_PATH "/usr/local/lib64"
ENV SPACEXPANSEGAME_DIR "/spacexpansegame"
LABEL description="SpaceXpanseships GSP with Charon"
VOLUME ["/log"]
ENV GLOG_log_dir "/log"
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
