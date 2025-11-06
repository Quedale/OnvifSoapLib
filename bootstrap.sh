#!/bin/bash
SKIP=0
NO_DOWNLOAD=0

#Save current working directory to run configure in
WORK_DIR=$(pwd)

#Get project root directory based on autogen.sh file location
SCRT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)
SUBPROJECT_DIR=${SUBPROJECT_DIR:="$SCRT_DIR/subprojects"}

#Cache folder for downloaded sources
SRC_CACHE_DIR=$SUBPROJECT_DIR/.cache

#meson utility location set for pip source
MESON_TOOL=meson

#Failure marker
FAILED=0

SKIP_GSOAP=0
SKIP_DEP=0
SKIP_WSDL=0
GSOAP_SRC_DIR="${GSOAP_SRC_DIR:=subprojects/gsoap-2.8}" 
GENERATED_SOURCE_PATH="${GENERATED_SOURCE_PATH:=$SCRT_DIR/src/generated}" 

i=1;
for arg in "$@" 
do
    if [ $arg == "--skip-gsoap" ]; then
        SKIP_GSOAP=1
    fi
    if [ $arg == "--skip-dependencies" ]; then
        SKIP_DEP=1
    fi
    if [ $arg == "--skip-wsdl" ]; then
        SKIP_WSDL=1
    fi
    if [ "$arg" == "--no-download" ]; then
        NO_DOWNLOAD=1
    fi
    i=$((i + 1));
done

# Define color code constants
ORANGE='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

############################################
#
# Function to print time for human
#
############################################
function displaytime {
  local T=$1
  local D=$((T/60/60/24))
  local H=$((T/60/60%24))
  local M=$((T/60%60))
  local S=$((T%60))
  printf "${ORANGE}*****************************\n${NC}"
  printf "${ORANGE}*** "
  (( $D > 0 )) && printf '%d days ' $D
  (( $H > 0 )) && printf '%d hours ' $H
  (( $M > 0 )) && printf '%d minutes ' $M
  (( $D > 0 || $H > 0 || $M > 0 )) && printf 'and '
  printf "%d seconds\n${NC}" $S
  printf "${ORANGE}*****************************\n${NC}"
}

############################################
#
# Function to download and extract tar package file
# Can optionally use a caching folder defined with SRC_CACHE_DIR
#
############################################
downloadAndExtract (){
  local path file # reset first
  local "${@}"

  if [ -z "${forcedownload}" ] && [ $NO_DOWNLOAD -eq 1 ]; then
    # printError msg="Download disabled. Missing dependency $file"
    FAILED=1
    return;
  fi

  if [ $SKIP -eq 1 ]
  then
      printf "${ORANGE}*****************************\n${NC}"
      printf "${ORANGE}*** Skipping Download ${path} ***\n${NC}"
      printf "${ORANGE}*****************************\n${NC}"
      return
  fi

  dest_val=""
  if [ ! -z "$SRC_CACHE_DIR" ]; then
    dest_val="$SRC_CACHE_DIR/${file}"
  else
    dest_val=${file}
  fi

  if [ ! -f "$dest_val" ]; then
    printf "${ORANGE}*****************************\n${NC}"
    printf "${ORANGE}*** Downloading : ${path} ***\n${NC}"
    printf "${ORANGE}*** Destination : $dest_val ***\n${NC}"
    printf "${ORANGE}*****************************\n${NC}"
    wget ${path} -O $dest_val
  else
    printf "${ORANGE}*****************************\n${NC}"
    printf "${ORANGE}*** Source already downloaded : ${path} ***\n${NC}"
    printf "${ORANGE}*** Destination : $dest_val ***\n${NC}"
    printf "${ORANGE}*****************************\n${NC}"
  fi

  printf "${ORANGE}*****************************\n${NC}"
  printf "${ORANGE}*** Extracting : ${file} ***\n${NC}"
  printf "${ORANGE}*****************************\n${NC}"
  if [[ $dest_val == *.tar.gz ]]; then
    tar xfz $dest_val
  elif [[ $dest_val == *.tar.xz ]]; then
    tar xf $dest_val
  elif [[ $dest_val == *.tar.bz2 ]]; then
    tar xjf $dest_val
  elif [[ $dest_val == *.zip ]]; then
    unzip -o $dest_val
  else
    echo "ERROR FILE NOT FOUND ${path} // ${file} // $dest_val"
    FAILED=1
  fi
}

############################################
#
# Function to clone a git repository or pull if it already exists locally
# Can optionally use a caching folder defined with SRC_CACHE_DIR
#
############################################
pullOrClone (){
  local path tag depth recurse ignorecache # reset first
  local "${@}"

  if [ -z "${forcedownload}" ] && [ $NO_DOWNLOAD -eq 1 ]; then
    # printError msg="Download disabled. Missing dependency $file"
    FAILED=1
    return;
  fi

  if [ $SKIP -eq 1 ]
  then
      printf "${ORANGE}*****************************\n${NC}"
      printf "${ORANGE}*** Skipping Pull/Clone ${tag}@${path} ***\n${NC}"
      printf "${ORANGE}*****************************\n${NC}"
      return
  fi

  recursestr=""
  if [ ! -z "${recurse}" ] 
  then
    recursestr="--recurse-submodules"
  fi
  depthstr=""
  if [ ! -z "${depth}" ] 
  then
    depthstr="--depth ${depth}"
  fi 

  tgstr=""
  tgstr2=""
  if [ ! -z "${tag}" ] 
  then
    tgstr="origin tags/${tag}"
    tgstr2="-b ${tag}"
  fi

  printf "${ORANGE}*****************************\n${NC}"
  printf "${ORANGE}*** Cloning ${tag}@${path} ***\n${NC}"
  printf "${ORANGE}*****************************\n${NC}"
  IFS='/' read -ra ADDR <<< "$path"
  namedotgit=${ADDR[-1]}
  IFS='.' read -ra ADDR <<< "$namedotgit"
  name=${ADDR[0]}

  dest_val=""
  if [ ! -z "$SRC_CACHE_DIR" ] && [ -z "${ignorecache}" ]; then
    dest_val="$SRC_CACHE_DIR/$name"
  else
    dest_val=$name
  fi
  if [ ! -z "${tag}" ]; then
    dest_val+="-${tag}"
  fi

  if [ -z "$SRC_CACHE_DIR" ] || [ -z "${ignorecache}" ]; then 
    #TODO Check if it's the right tag
    git -C $dest_val pull $tgstr 2> /dev/null || git clone -j$(nproc) $recursestr $depthstr $tgstr2 ${path} $dest_val
  elif [ -d "$dest_val" ] && [ ! -z "${tag}" ]; then #Folder exist, switch to tag
    currenttag=$(git -C $dest_val tag --points-at ${tag})
    echo "TODO Check current tag \"${tag}\" == \"$currenttag\""
    git -C $dest_val pull $tgstr 2> /dev/null || git clone -j$(nproc) $recursestr $depthstr $tgstr2 ${path} $dest_val
  elif [ -d "$dest_val" ] && [ -z "${tag}" ]; then #Folder exist, switch to main
    currenttag=$(git -C $dest_val tag --points-at ${tag})
    echo "TODO Handle no tag \"${tag}\" == \"$currenttag\""
    git -C $dest_val pull $tgstr 2> /dev/null || git clone -j$(nproc) $recursestr $depthstr $tgstr2 ${path} $dest_val
  elif [ ! -d "$dest_val" ]; then #fresh start
    git -C $dest_val pull $tgstr 2> /dev/null || git clone -j$(nproc) $recursestr $depthstr $tgstr2 ${path} $dest_val
  else
    if [ -d "$dest_val" ]; then
      echo "yey destval"
    fi
    if [ -z "${tag}" ]; then
      echo "yey tag"
    fi
    echo "1 $dest_val : $(test -f \"$dest_val\")"
    echo "2 ${tag} : $(test -z \"${tag}\")"
  fi

  if [ ! -z "$SRC_CACHE_DIR" ] && [ -z "${ignorecache}" ]; then
    printf "${ORANGE}*****************************\n${NC}"
    printf "${ORANGE}*** Copy repo from cache ***\n${NC}"
    printf "${ORANGE}*** $dest_val ***\n${NC}"
    printf "${ORANGE}*****************************\n${NC}"
    rm -rf $name
    cp -r $dest_val ./$name
  fi
}

############################################
#
# Function to build a project configured with autotools
#
############################################
buildMakeProject(){
  local srcdir prefix autogen autoreconf configure make cmakedir cmakeargs installargs bootstrap configcustom
  local "${@}"

  build_start=$SECONDS
  if [ $SKIP -eq 1 ]; then
      printf "${ORANGE}*****************************\n${NC}"
      printf "${ORANGE}*** Skipping Make ${srcdir} ***\n${NC}"
      printf "${ORANGE}*****************************\n${NC}"
      return
  fi

  printf "${ORANGE}*****************************\n${NC}"
  printf "${ORANGE}* Building Project ***\n${NC}"
  printf "${ORANGE}* Src dir : ${srcdir} ***\n${NC}"
  printf "${ORANGE}* Prefix : ${prefix} ***\n${NC}"
  printf "${ORANGE}*****************************\n${NC}"

  curr_dir=$(pwd)
  cd ${srcdir}

  if [ -f "./bootstrap" ]; then
    printf "${ORANGE}*****************************\n${NC}"
    printf "${ORANGE}*** bootstrap ${srcdir} ***\n${NC}"
    printf "${ORANGE}*****************************\n${NC}"
    ./bootstrap ${bootstrap}
    status=$?
    if [ $status -ne 0 ]; then
        printf "${RED}*****************************\n${NC}"
        printf "${RED}*** ./bootstrap failed ${srcdir} ***\n${NC}"
        printf "${RED}*****************************\n${NC}"
        FAILED=1
        cd $curr_dir
        return
    fi
  fi

  if [ -f "./bootstrap.sh" ]; then
    printf "${ORANGE}*****************************\n${NC}"
    printf "${ORANGE}*** bootstrap.sh ${srcdir} ***\n${NC}"
    printf "${ORANGE}*****************************\n${NC}"
    ./bootstrap.sh ${bootstrap}
    status=$?
    if [ $status -ne 0 ]; then
        printf "${RED}*****************************\n${NC}"
        printf "${RED}*** ./bootstrap.sh failed ${srcdir} ***\n${NC}"
        printf "${RED}*****************************\n${NC}"
        FAILED=1
        cd $curr_dir
        return
    fi
  fi
  if [ -f "./autogen.sh" ] && [ "${autogen}" != "skip" ]; then
    printf "${ORANGE}*****************************\n${NC}"
    printf "${ORANGE}*** autogen ${srcdir} ***\n${NC}"
    printf "${ORANGE}*****************************\n${NC}"
    ./autogen.sh ${autogen}
    status=$?
    if [ $status -ne 0 ]; then
        printf "${RED}*****************************\n${NC}"
        printf "${RED}*** Autogen failed ${srcdir} ***\n${NC}"
        printf "${RED}*****************************\n${NC}"
        FAILED=1
        cd $curr_dir
        return
    fi
  fi

  if [ ! -z "${autoreconf}" ] 
  then
    printf "${ORANGE}*****************************\n${NC}"
    printf "${ORANGE}*** autoreconf ${srcdir} ***\n${NC}"
    printf "${ORANGE}*****************************\n${NC}"
    autoreconf ${autoreconf}
    status=$?
    if [ $status -ne 0 ]; then
        printf "${RED}*****************************\n${NC}"
        printf "${RED}*** Autoreconf failed ${srcdir} ***\n${NC}"
        printf "${RED}*****************************\n${NC}"
        FAILED=1
        cd $curr_dir
        return
    fi
  fi
  if [ ! -z "${cmakedir}" ] 
  then
    printf "${ORANGE}*****************************\n${NC}"
    printf "${ORANGE}*** cmake ${srcdir} ***\n${NC}"
    printf "${ORANGE}*** Args ${cmakeargs} ***\n${NC}"
    printf "${ORANGE}*****************************\n${NC}"
    cmake -G "Unix Makefiles" \
      ${cmakeargs} \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX="${prefix}" \
      -DENABLE_TESTS=OFF \
      -DENABLE_SHARED=on \
      "${cmakedir}"
    status=$?
    if [ $status -ne 0 ]; then
      printf "${RED}*****************************\n${NC}"
      printf "${RED}*** CMake failed ${srcdir} ***\n${NC}"
      printf "${RED}*****************************\n${NC}"
      FAILED=1
      cd $curr_dir
      return
    fi
  fi

  if [ ! -z "${configcustom}" ]; then
    printf "${ORANGE}*****************************\n${NC}"
    printf "${ORANGE}*** custom config ${srcdir} ***\n${NC}"
    printf "${ORANGE}*** ${configcustom} ***\n${NC}"
    printf "${ORANGE}*****************************\n${NC}"
      bash -c "${configcustom}"
    status=$?
    if [ $status -ne 0 ]; then
      printf "${RED}*****************************\n${NC}"
      printf "${RED}*** Custom Config failed ${srcdir} ***\n${NC}"
      printf "${RED}*****************************\n${NC}"
      FAILED=1
      cd $curr_dir
      return
    fi
  fi

  if [ -f "./configure" ] && [ -z "${cmakedir}" ]; then
    printf "${ORANGE}*****************************\n${NC}"
    printf "${ORANGE}*** configure ${srcdir} ***\n${NC}"
    printf "${ORANGE}*****************************\n${NC}"
    ./configure \
        --prefix=${prefix} \
        ${configure}
    status=$?
    if [ $status -ne 0 ]; then
      printf "${RED}*****************************\n${NC}"
      printf "${RED}*** ./configure failed ${srcdir} ***\n${NC}"
      printf "${RED}*****************************\n${NC}"
      FAILED=1
      cd $curr_dir
      return
    fi
  else
    printf "${ORANGE}*****************************\n${NC}"
    printf "${ORANGE}*** no configuration available ${srcdir} ***\n${NC}"
    printf "${ORANGE}*****************************\n${NC}"
  fi

  printf "${ORANGE}*****************************\n${NC}"
  printf "${ORANGE}*** compile ${srcdir} ***\n${NC}"
  printf "${ORANGE}*** Make Args : ${makeargs} ***\n${NC}"
  printf "${ORANGE}*****************************\n${NC}"
  make -j$(nproc) ${make}
  status=$?
  if [ $status -ne 0 ]; then
      printf "${RED}*****************************\n${NC}"
      printf "${RED}*** Make failed ${srcdir} ***\n${NC}"
      printf "${RED}*****************************\n${NC}"
      FAILED=1
      cd $curr_dir
      return
  fi


  printf "${ORANGE}*****************************\n${NC}"
  printf "${ORANGE}*** install ${srcdir} ***\n${NC}"
  printf "${ORANGE}*** Make Args : ${makeargs} ***\n${NC}"
  printf "${ORANGE}*****************************\n${NC}"
  make -j$(nproc) ${make} install ${installargs}
  status=$?
  if [ $status -ne 0 ]; then
      printf "${RED}*****************************\n${NC}"
      printf "${RED}*** Make failed ${srcdir} ***\n${NC}"
      printf "${RED}*****************************\n${NC}"
      FAILED=1
      cd $curr_dir
      return
  fi

  build_time=$(( SECONDS - build_start ))
  displaytime $build_time

  cd $curr_dir
}


############################################
#
# Function to build a project configured with meson
#
############################################
buildMesonProject() {
  local srcdir mesonargs prefix setuppatch bindir destdir builddir defaultlib clean
  local "${@}"

  build_start=$SECONDS
  if [ $SKIP -eq 1 ]
  then
      printf "${ORANGE}*****************************\n${NC}"
      printf "${ORANGE}*** Skipping Meson ${srcdir} ***\n${NC}"
      printf "${ORANGE}*****************************\n${NC}"
      return
  fi

  printf "${ORANGE}*****************************\n${NC}"
  printf "${ORANGE}* Building Github Project ***\n${NC}"
  printf "${ORANGE}* Src dir : ${srcdir} ***\n${NC}"
  printf "${ORANGE}* Prefix : ${prefix} ***\n${NC}"
  printf "${ORANGE}*****************************\n${NC}"

  curr_dir=$(pwd)

  build_dir=""
  if [ ! -z "${builddir}" ] 
  then
    build_dir="${builddir}"
  else
    build_dir="build_dir"
  fi

  default_lib=""
  if [ ! -z "${defaultlib}" ] 
  then
    default_lib="${defaultlib}"
  else
    default_lib="static"
  fi

  bindir_val=""
  if [ ! -z "${bindir}" ]; then
      bindir_val="--bindir=${bindir}"
  fi
  
  if [ ! -z "${clean}" ]; then
    rm -rf ${srcdir}/$build_dir
  fi

  if [ ! -d "${srcdir}/$build_dir" ]; then
      mkdir -p ${srcdir}/$build_dir

      cd ${srcdir}
      if [ -d "./subprojects" ]; then
          printf "${ORANGE}*****************************\n${NC}"
          printf "${ORANGE}*** Download Subprojects ${srcdir} ***\n${NC}"
          printf "${ORANGE}*****************************\n${NC}"
          #     $MESON_TOOL subprojects download
      fi

      echo "setup patch : ${setuppatch}"
      if [ ! -z "${setuppatch}" ]; then
          printf "${ORANGE}*****************************\n${NC}"
          printf "${ORANGE}*** Meson Setup Patch ${srcdir} ***\n${NC}"
          printf "${ORANGE}*** ${setuppatch} ***\n${NC}"
          printf "${ORANGE}*****************************\n${NC}"
          bash -c "${setuppatch}"
          status=$?
          if [ $status -ne 0 ]; then
              printf "${RED}*****************************\n${NC}"
              printf "${RED}*** Bash Setup failed ${srcdir} ***\n${NC}"
              printf "${RED}*****************************\n${NC}"
              FAILED=1
              cd $curr_dir
              return
          fi
      fi

      printf "${ORANGE}*****************************\n${NC}"
      printf "${ORANGE}*** Meson Setup ${srcdir} ***\n${NC}"
      printf "${ORANGE}*****************************\n${NC}"
      $MESON_TOOL setup $build_dir \
          ${mesonargs} \
          --default-library=$default_lib \
          --prefix=${prefix} \
          $bindir_val \
          --libdir=lib \
          --includedir=include \
          --buildtype=release 
      status=$?
      if [ $status -ne 0 ]; then
          printf "${RED}*****************************\n${NC}"
          printf "${RED}*** Meson Setup failed ${srcdir} ***\n${NC}"
          printf "${RED}*****************************\n${NC}"
          rm -rf $build_dir
          FAILED=1
          cd $curr_dir
          return
      fi
  else
      cd ${srcdir}
      if [ -d "./subprojects" ]; then
          printf "${ORANGE}*****************************\n${NC}"
          printf "${ORANGE}*** Meson Update ${srcdir} ***\n${NC}"
          printf "${ORANGE}*****************************\n${NC}"
          #     $MESON_TOOL subprojects update
      fi

      if [ ! -z "${setuppatch}" ]; then
          printf "${ORANGE}*****************************\n${NC}"
          printf "${ORANGE}*** Meson Setup Patch ${srcdir} ***\n${NC}"
          printf "${ORANGE}*** ${setuppatch} ***\n${NC}"
          printf "${ORANGE}*****************************\n${NC}"
          bash -c "${setuppatch}"
          status=$?
          if [ $status -ne 0 ]; then
              printf "${RED}*****************************\n${NC}"
              printf "${RED}*** Bash Setup failed ${srcdir} ***\n${NC}"
              printf "${RED}*****************************\n${NC}"
              FAILED=1
              cd $curr_dir
              return
          fi
      fi

      printf "${ORANGE}*****************************\n${NC}"
      printf "${ORANGE}*** Meson Reconfigure $(pwd) ${srcdir} ***\n${NC}"
      printf "${ORANGE}*****************************\n${NC}"
      $MESON_TOOL setup $build_dir \
          ${mesonargs} \
          --default-library=$default_lib \
          --prefix=${prefix} \
          $bindir_val \
          --libdir=lib \
          --includedir=include \
          --buildtype=release \
          --reconfigure

      status=$?
      if [ $status -ne 0 ]; then
          printf "${RED}*****************************\n${NC}"
          printf "${RED}*** Meson Setup failed ${srcdir} ***\n${NC}"
          printf "${RED}*****************************\n${NC}"
          rm -rf $build_dir
          FAILED=1
          cd $curr_dir
          return
      fi
  fi

  printf "${ORANGE}*****************************\n${NC}"
  printf "${ORANGE}*** Meson Compile ${srcdir} ***\n${NC}"
  printf "${ORANGE}*****************************\n${NC}"
  $MESON_TOOL compile -C $build_dir
  status=$?
  if [ $status -ne 0 ]; then
      printf "${RED}*****************************\n${NC}"
      printf "${RED}*** meson compile failed ${srcdir} ***\n${NC}"
      printf "${RED}*****************************\n${NC}"
      FAILED=1
      cd $curr_dir
      return
  fi

  printf "${ORANGE}*****************************\n${NC}"
  printf "${ORANGE}*** Meson Install ${srcdir} ***\n${NC}"
  printf "${ORANGE}*****************************\n${NC}"
  DESTDIR=${destdir} $MESON_TOOL install -C $build_dir
  status=$?
  if [ $status -ne 0 ]; then
      printf "${RED}*****************************\n${NC}"
      printf "${RED}*** Meson Install failed ${srcdir} ***\n${NC}"
      printf "${RED}*****************************\n${NC}"
      FAILED=1
      cd $curr_dir
      return
  fi

  build_time=$(( SECONDS - build_start ))
  displaytime $build_time
  cd $curr_dir

}

############################################
#
# Function to check if a program exists
#
############################################
checkProg () {
  local name args path # reset first
  local "${@}"

  if !PATH=$PATH:${path} command -v ${name} &> /dev/null
  then
    return #Prog not found
  else
    PATH=$PATH:${path} command ${name} ${args} &> /dev/null
    status=$?
    if [[ $status -eq 0 ]]; then
        echo "Working"
    else
        return #Prog failed
    fi
  fi
}

# Hard dependency check
MISSING_DEP=0
if [ -z "$(checkProg name='make' args='--version' path=$PATH)" ]; then
 MISSING_DEP=1
 echo "make build utility not found! Aborting..."
fi

if [ -z "$(checkProg name='automake' args='--version' path=$PATH)" ]; then
  MISSING_DEP=1
  echo "automake build utility not found! Aborting..."
fi

if [ -z "$(checkProg name='autoconf' args='--version' path=$PATH)" ]; then
  MISSING_DEP=1
  echo "autoconf build utility not found! Aborting..."
fi

if [ -z "$(checkProg name='pkg-config' args='--version' path=$PATH)" ]; then
  MISSING_DEP=1
  echo "pkg-config build utility not found! Aborting..."
fi

if [ -z "$(checkProg name='gcc' args='--version' path=$PATH)" ]; then
 MISSING_DEP=1
 echo "gcc build utility not found! Aborting..."
fi

if [ $MISSING_DEP -eq 1 ]; then
  exit 1
fi

#Change to the project folder to run autoconf and automake
cd $SCRT_DIR

mkdir -p $SUBPROJECT_DIR
mkdir -p $SRC_CACHE_DIR

cd $SUBPROJECT_DIR

################################################################
# 
#    Build cutils
#       
################################################################
#Check if new changes needs to be pulled
ret=0
if [ ${SKIP_DEP} -eq 0 ]; then
  git -C CUtils remote update 2> /dev/null
  LOCAL=$(git -C CUtils rev-parse @ 2> /dev/null)
  REMOTE=$(git -C CUtils rev-parse @{u} 2> /dev/null)
  BASE=$(git -C CUtils merge-base @ @{u} 2> /dev/null)
  force_rebuild=0
  if [ $LOCAL = $REMOTE ]; then
      echo "CUtils is already up-to-date. Do nothing..."
  elif [ $LOCAL = $BASE ]; then
      echo "CUtils has new changes. Force rebuild..."
      force_rebuild=1
  elif [ $REMOTE = $BASE ]; then
      echo "CUtils has local changes. Doing nothing..."
  else
      echo "Error CUtils is diverged."
      exit 1
  fi

  if [ $force_rebuild -eq 0 ]; then
    CUTILSLIB_PKG=$SUBPROJECT_DIR/CUtils/build/dist/lib/pkgconfig
    PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$CUTILSLIB_PKG \
    pkg-config --exists --print-errors "cutils"
    ret=$?
  else
    ret=1
  fi
fi

if [ $ret != 0 ]; then
  pullOrClone path=https://github.com/Quedale/CUtils.git ignorecache="true"
  if [ $FAILED -eq 1 ]; then exit 1; fi
  buildMakeProject srcdir="CUtils" prefix="$SUBPROJECT_DIR/CUtils/build/dist" cmakedir="." outoftree=true
  if [ $FAILED -eq 1 ]; then exit 1; fi
else
  echo "CUtils already found."
fi

################################################################
# 
#    Build gSoap
#       
################################################################
PATH=$SUBPROJECT_DIR/gsoap-2.8/build/dist/bin:$PATH
ret=0
if [ ${SKIP_DEP} -eq 0 ]; then
  GSOAP_PKG=$SUBPROJECT_DIR/gsoap-2.8/build/dist/lib/pkgconfig
  PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$GSOAP_PKG \
  pkg-config --exists --print-errors "gsoap >= 2.8.123"
  ret=$?
fi

if [ $SKIP_GSOAP -eq 0 ] && [ $ret != 0 ]; then

    ################################################################
    # 
    #    Build unzip for gsoap
    #       
    ################################################################
    PATH=$SUBPROJECT_DIR/unzip60/build/dist/bin:$PATH
    if [ -z "$(checkProg name='unzip' args='-v' path=$PATH)" ]; then
        echo "-- Building unzip --"
        downloadAndExtract file="unzip60.tar.gz" path="https://sourceforge.net/projects/infozip/files/UnZip%206.x%20%28latest%29/UnZip%206.0/unzip60.tar.gz/download"
        if [ $FAILED -eq 1 ]; then exit 1; fi
        buildMakeProject srcdir="unzip60" make="-f unix/Makefile generic" installargs="prefix=$SUBPROJECT_DIR/unzip60/build/dist MANDIR=$SUBPROJECT_DIR/unzip60/build/dist/share/man/man1 -f unix/Makefile"
        if [ $FAILED -eq 1 ]; then exit 1; fi
    fi

    gsoap_version=2.8.134
    echo "-- Building gsoap libgsoap-dev --"
    #WS-Security depends on OpenSSL library 3.0 or 1.1
    downloadAndExtract file="gsoap.zip" path="https://sourceforge.net/projects/gsoap2/files/gsoap_$gsoap_version.zip/download"
    if [ $FAILED -eq 1 ]; then exit 1; fi
    PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$OPENSSL_PKG:$ZLIB_PKG \
    C_INCLUDE_PATH="$SSL_INCLUDE:$ZLIB_INCLUDE:$C_INCLUDE_PATH" \
    CPLUS_INCLUDE_PATH="$SSL_INCLUDE:$ZLIB_INCLUDE:$CPLUS_INCLUDE_PATH" \
    LIBRARY_PATH="$(pkg-config --variable=libdir openssl):$LIBRARY_PATH" \
    LD_LIBRARY_PATH="$(pkg-config --variable=libdir openssl):$LD_LIBRARY_PATH" \
    LIBS='-ldl -lpthread' \
    buildMakeProject srcdir="gsoap-2.8" prefix="$SUBPROJECT_DIR/gsoap-2.8/build/dist" autogen="skip" configure="--with-openssl=/usr/lib/ssl"
    if [ $FAILED -eq 1 ]; then exit 1; fi
fi


ret=0
if [ ${SKIP_DEP} -eq 0 ]; then
  NTLM_PKG=$SUBPROJECT_DIR/libntlm/build/dist/lib/pkgconfig
  PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$NTLM_PKG \
  pkg-config --exists --print-errors "libntlm >= 1.5"
  ret=$?
fi
if [ $ret != 0 ]; then
  pullOrClone path=https://gitlab.com/gsasl/libntlm.git tag=v1.7
  if [ $FAILED -eq 1 ]; then exit 1; fi
  buildMakeProject srcdir="libntlm" prefix="$SUBPROJECT_DIR/libntlm/build/dist" configure="--enable-shared=no" #TODO support shared linking
  if [ $FAILED -eq 1 ]; then exit 1; fi
fi

# This option still required internet to reach oasis schemes.
# no internet requirement to support flatpak builder
# downloadAndExtract file="24.06.tar.gz" path="https://github.com/onvif/specs/archive/refs/tags/24.06.tar.gz"
# if [ $FAILED -eq 1 ]; then exit 1; fi

#Get out of subproject folder
cd ..

if [ $SKIP_WSDL -eq 0 ]; then
    echo "Generating WSDL gsoap files..."
    # rm -rf $GENERATED_SOURCE_PATH # Removed since path can be set externally and we don't want to nuke the wrong folder
    mkdir -p $GENERATED_SOURCE_PATH

    wsdl2h -Ow4 -t$SUBPROJECT_DIR/../wsdl/typemap.dat -o $GENERATED_SOURCE_PATH/onvif.h -c \
      ./wsdl/onvif/wsdl/devicemgmt.wsdl \
      ./wsdl/onvif/wsdl/event.wsdl \
      ./wsdl/onvif/wsdl/deviceio.wsdl \
      ./wsdl/onvif/wsdl/imaging.wsdl \
      ./wsdl/onvif/wsdl/media.wsdl \
      ./wsdl/onvif/wsdl/media2.wsdl \
      ./wsdl/onvif/wsdl/ptz.wsdl \
      ./wsdl/onvif/wsdl/remotediscovery.wsdl \
      ./wsdl/onvif/wsdl/advancedsecurity.wsdl
      # Use bellow if using github ONVIF specs
      # ./subprojects/specs-24.06/wsdl/ver10/device/wsdl/devicemgmt.wsdl \
      # ./subprojects/specs-24.06/wsdl/ver10/events/wsdl/event.wsdl \
      # ./subprojects/specs-24.06/wsdl/ver10/deviceio.wsdl \
      # ./subprojects/specs-24.06/wsdl/ver20/imaging/wsdl/imaging.wsdl \
      # ./subprojects/specs-24.06/wsdl/ver10/media/wsdl/media.wsdl \
      # ./subprojects/specs-24.06/wsdl/ver20/media/wsdl/media.wsdl \
      # ./subprojects/specs-24.06/wsdl/ver20/ptz/wsdl/ptz.wsdl \
      # ./subprojects/specs-24.06/wsdl/ver10/advancedsecurity/wsdl/advancedsecurity.wsdl \
      
      # Use below for ONVIF schema from onvif.org
      # http://schemas.xmlsoap.org/ws/2005/04/discovery/ws-discovery.wsdl #ONVIF remotediscovery.wsdl is outdated https://github.com/onvif/specs/issues/319
      # http://www.onvif.org/onvif/ver10/device/wsdl/devicemgmt.wsdl \
      # http://www.onvif.org/onvif/ver10/events/wsdl/event.wsdl \
      # http://www.onvif.org/onvif/ver10/deviceio.wsdl \
      # http://www.onvif.org/onvif/ver20/imaging/wsdl/imaging.wsdl \
      # http://www.onvif.org/onvif/ver10/media/wsdl/media.wsdl \
      # http://www.onvif.org/onvif/ver20/media/wsdl/media.wsdl \
      # http://www.onvif.org/onvif/ver20/ptz/wsdl/ptz.wsdl \
      # http://www.onvif.org/onvif/ver10/network/wsdl/remotediscovery.wsdl \
      # http://www.onvif.org/ver10/advancedsecurity/wsdl/advancedsecurity.wsdl
    ret=$?
    if [ $ret != 0 ]; then
      printf "${RED}*****************************\n${NC}"
      printf "${RED}*** Failed to generate gsoap C header file ${srcdir} ***\n${NC}"
      printf "${RED}*****************************\n${NC}"
      exit 1;
    fi
    # http://www.onvif.org/onvif/ver10/display.wsdl \
    # http://www.onvif.org/onvif/ver10/receiver.wsdl \
    # http://www.onvif.org/onvif/ver10/recording.wsdl \
    # http://www.onvif.org/onvif/ver10/search.wsdl \
    # http://www.onvif.org/onvif/ver10/replay.wsdl \
    # http://www.onvif.org/onvif/ver20/analytics/wsdl/analytics.wsdl \
    # http://www.onvif.org/onvif/ver10/analyticsdevice.wsdl \ 
    soapcpp2 -n -ponvifsoap -f100 -CL -x -I$GSOAP_SRC_DIR/gsoap/import:$GSOAP_SRC_DIR/gsoap  $GENERATED_SOURCE_PATH/onvif.h -d$GENERATED_SOURCE_PATH
    ret=$?
    if [ $ret != 0 ]; then
      printf "${RED}*****************************\n${NC}"
      printf "${RED}*** Failed to generate gsoap C code ${srcdir} ***\n${NC}"
      printf "${RED}*****************************\n${NC}"
      exit 1;
    fi
else
    echo "Skipping WSDL class generation..."
fi
