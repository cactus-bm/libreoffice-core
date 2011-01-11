#*************************************************************************

# extend for JDK include (seems only needed in setsolar env?)
SOLARINC += $(JDKINCS)

OUTDIR := $(SOLARVERSION)/$(INPATH)
WORKDIR := $(SOLARVERSION)/$(INPATH)/workdir

# if the goals do not include the targets install or uninstall we check if a
# local overlay build dir was requested. We are making an distiction between
# partitial and full builds. gb_LOCALPARTIALBUILDDIR is only used here,
# later only gb_LOCALBUILDDIR is used.
ifeq ($(filter install uninstall,$(MAKECMDGOALS)),)
ifeq ($(gb_PARTIALBUILD),$(true))
ifneq ($(gb_LOCALPARTIALBUILDDIR),)
gb_LOCALBUILDDIR := $(gb_LOCALPARTIALBUILDDIR)
OUTDIR := $(gb_LOCALBUILDDIR)/outdir/$(INPATH)
WORKDIR := $(gb_LOCALBUILDDIR)/workdir/$(INPATH)
else
gb_LOCALBUILDDIR :=
endif
else
ifneq ($(gb_LOCALBUILDDIR),)
OUTDIR := $(gb_LOCALBUILDDIR)/outdir/$(INPATH)
WORKDIR := $(gb_LOCALBUILDDIR)/workdir/$(INPATH)
endif
endif
endif

ifneq ($(gb_LOCALBUILDDIR),)

ifneq ($(gb_DISABLELOCALBUILD),)
$(error This is requesting a local build. This scenario has been disable because the environment is not correctly setup for it.)
endif

.PHONY : setuplocal
setuplocal :
	mkdir -p $(OUTDIR) $(WORKDIR)
	time rsync -a $(SOLARVERSION)/$(INPATH)/ $(OUTDIR)

endif

ifeq ($(strip $(gb_REPOS)),)
gb_REPOS := $(SOLARSRC)
endif
SRCDIR := $(firstword $(gb_REPOS))

# HACK
# unixify windoze paths
ifeq ($(OS),WNT)
SRCDIR := $(shell cygpath -u $(SRCDIR))
WORKDIR := $(shell cygpath -u $(WORKDIR))
OUTDIR := $(shell cygpath -u $(OUTDIR))
gb_REPOS := $(shell cygpath -u $(gb_REPOS))
endif

REPODIR := $(patsubst %/,%,$(dir $(firstword $(gb_REPOS))))

# vim: set noet sw=4 ts=4:
