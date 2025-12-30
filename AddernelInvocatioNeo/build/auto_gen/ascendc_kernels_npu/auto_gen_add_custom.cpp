#ifndef __ADD_CUSTOM__KERNEL_FUN_H__
#define __ADD_CUSTOM__KERNEL_FUN_H__

#undef __global__
#define __global__ inline
#define add_custom add_custom_origin
#include "/home/ma-user/work/ww/ascendc_samples/operator/ascendc/0_introduction/3_add_kernellaunch/AddKernelInvocationNeo/add_custom.cpp"

#undef add_custom
#undef __global__
#if ASCENDC_CPU_DEBUG
#define __global__
#else
#define __global__ __attribute__((cce_kernel))
#endif

#ifndef ONE_CORE_DUMP_SIZE
#define ONE_CORE_DUMP_SIZE 1048576 * 1
#endif

extern "C" __global__ [aicore] void auto_gen_add_custom_kernel(
#if defined ASCENDC_DUMP || defined ASCENDC_TIME_STAMP_ON
GM_ADDR dumpAddr,
#endif
__attribute__((cce_global)) uint8_t* x, __attribute__((cce_global)) uint8_t* y, __attribute__((cce_global)) uint8_t* z, GM_ADDR overflow_status) {
#if defined ASCENDC_DUMP || defined ASCENDC_TIME_STAMP_ON
    AscendC::InitDump(false, dumpAddr, ONE_CORE_DUMP_SIZE);
#ifdef ASCENDC_TIME_STAMP_ON
    AscendC::PrintTimeStamp(static_cast<uint32_t>(AscendC::TimeStampId::TIME_STAMP_WRAP_INIT_DUMP));
#endif
#endif

#ifdef ASCENDC_DUMP
    uint64_t __ascendc_tStamp = 0;
    uint64_t __ascendc_version = 0;
     __gm__ char* __ascendc_versionStr = nullptr;
    GetCannVersion(__ascendc_versionStr, __ascendc_version, __ascendc_tStamp);
    if (__ascendc_tStamp == 0) {
        AscendC::printf("[WARNING]: CANN TimeStamp is invalid, CANN TimeStamp is %u\n", __ascendc_tStamp);
    } else {
        AscendC::printf("CANN Version: %s, TimeStamp: %u\n", (__gm__ const char*)(__ascendc_versionStr), __ascendc_tStamp);
    }
#endif
#if defined(HAVE_WORKSPACE)
    GM_ADDR workspace_param;
    GM_ADDR workspace_usr;
#if defined(HAVE_TILING)
    workspace_param = y;
#else
    workspace_param = z;
#endif
    AscendC::SetSysWorkspaceForce(workspace_param);
    workspace_usr = AscendC::GetUserWorkspace(workspace_param);
#if defined(HAVE_TILING)
    y = workspace_usr;
#else
    z = workspace_usr;
#endif
#endif
    add_custom_origin(x, y, z);
#if defined(ASCENDC_DUMP) && defined(ASCENDC_DEBUG)
    AscendC::WriteBackOverflow(overflow_status);
#endif
}

#endif
