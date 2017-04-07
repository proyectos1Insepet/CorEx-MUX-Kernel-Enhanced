// ======================================================================
// CorEx-Mux-Kernel.v generated from TopDesign.cysch
// 04/06/2017 at 11:22
// This file is auto generated. ANY EDITS YOU MAKE MAY BE LOST WHEN THIS FILE IS REGENERATED!!!
// ======================================================================

/* -- WARNING: The following section of defines are deprecated and will be removed in a future release -- */
`define CYDEV_CHIP_DIE_LEOPARD 1
`define CYDEV_CHIP_REV_LEOPARD_PRODUCTION 3
`define CYDEV_CHIP_REV_LEOPARD_ES3 3
`define CYDEV_CHIP_REV_LEOPARD_ES2 1
`define CYDEV_CHIP_REV_LEOPARD_ES1 0
`define CYDEV_CHIP_DIE_TMA4 2
`define CYDEV_CHIP_REV_TMA4_PRODUCTION 17
`define CYDEV_CHIP_REV_TMA4_ES 17
`define CYDEV_CHIP_REV_TMA4_ES2 33
`define CYDEV_CHIP_DIE_PSOC4A 3
`define CYDEV_CHIP_REV_PSOC4A_PRODUCTION 17
`define CYDEV_CHIP_REV_PSOC4A_ES0 17
`define CYDEV_CHIP_DIE_PSOC5LP 4
`define CYDEV_CHIP_REV_PSOC5LP_PRODUCTION 0
`define CYDEV_CHIP_REV_PSOC5LP_ES0 0
`define CYDEV_CHIP_DIE_PANTHER 5
`define CYDEV_CHIP_REV_PANTHER_PRODUCTION 1
`define CYDEV_CHIP_REV_PANTHER_ES1 1
`define CYDEV_CHIP_REV_PANTHER_ES0 0
`define CYDEV_CHIP_DIE_EXPECT 4
`define CYDEV_CHIP_REV_EXPECT 0
`define CYDEV_CHIP_DIE_ACTUAL 4
/* -- WARNING: The previous section of defines are deprecated and will be removed in a future release -- */
`define CYDEV_CHIP_FAMILY_UNKNOWN 0
`define CYDEV_CHIP_MEMBER_UNKNOWN 0
`define CYDEV_CHIP_FAMILY_PSOC3 1
`define CYDEV_CHIP_MEMBER_3A 1
`define CYDEV_CHIP_REVISION_3A_PRODUCTION 3
`define CYDEV_CHIP_REVISION_3A_ES3 3
`define CYDEV_CHIP_REVISION_3A_ES2 1
`define CYDEV_CHIP_REVISION_3A_ES1 0
`define CYDEV_CHIP_FAMILY_PSOC4 2
`define CYDEV_CHIP_MEMBER_4G 2
`define CYDEV_CHIP_REVISION_4G_PRODUCTION 17
`define CYDEV_CHIP_REVISION_4G_ES 17
`define CYDEV_CHIP_REVISION_4G_ES2 33
`define CYDEV_CHIP_MEMBER_4U 3
`define CYDEV_CHIP_REVISION_4U_PRODUCTION 0
`define CYDEV_CHIP_MEMBER_4E 4
`define CYDEV_CHIP_REVISION_4E_PRODUCTION 0
`define CYDEV_CHIP_MEMBER_4N 5
`define CYDEV_CHIP_REVISION_4N_PRODUCTION 0
`define CYDEV_CHIP_MEMBER_4D 6
`define CYDEV_CHIP_REVISION_4D_PRODUCTION 0
`define CYDEV_CHIP_MEMBER_4J 7
`define CYDEV_CHIP_REVISION_4J_PRODUCTION 0
`define CYDEV_CHIP_MEMBER_4K 8
`define CYDEV_CHIP_REVISION_4K_PRODUCTION 0
`define CYDEV_CHIP_MEMBER_4H 9
`define CYDEV_CHIP_REVISION_4H_PRODUCTION 0
`define CYDEV_CHIP_MEMBER_4A 10
`define CYDEV_CHIP_REVISION_4A_PRODUCTION 17
`define CYDEV_CHIP_REVISION_4A_ES0 17
`define CYDEV_CHIP_MEMBER_4F 11
`define CYDEV_CHIP_REVISION_4F_PRODUCTION 0
`define CYDEV_CHIP_REVISION_4F_PRODUCTION_256K 0
`define CYDEV_CHIP_REVISION_4F_PRODUCTION_256DMA 0
`define CYDEV_CHIP_MEMBER_4F 12
`define CYDEV_CHIP_REVISION_4F_PRODUCTION 0
`define CYDEV_CHIP_REVISION_4F_PRODUCTION_256K 0
`define CYDEV_CHIP_REVISION_4F_PRODUCTION_256DMA 0
`define CYDEV_CHIP_MEMBER_4M 13
`define CYDEV_CHIP_REVISION_4M_PRODUCTION 0
`define CYDEV_CHIP_MEMBER_4L 14
`define CYDEV_CHIP_REVISION_4L_PRODUCTION 0
`define CYDEV_CHIP_MEMBER_4I 15
`define CYDEV_CHIP_REVISION_4I_PRODUCTION 0
`define CYDEV_CHIP_MEMBER_4C 16
`define CYDEV_CHIP_REVISION_4C_PRODUCTION 0
`define CYDEV_CHIP_FAMILY_PSOC5 3
`define CYDEV_CHIP_MEMBER_5B 17
`define CYDEV_CHIP_REVISION_5B_PRODUCTION 0
`define CYDEV_CHIP_REVISION_5B_ES0 0
`define CYDEV_CHIP_MEMBER_5A 18
`define CYDEV_CHIP_REVISION_5A_PRODUCTION 1
`define CYDEV_CHIP_REVISION_5A_ES1 1
`define CYDEV_CHIP_REVISION_5A_ES0 0
`define CYDEV_CHIP_FAMILY_USED 3
`define CYDEV_CHIP_MEMBER_USED 17
`define CYDEV_CHIP_REVISION_USED 0
// Component: cy_virtualmux_v1_0
`ifdef CY_BLK_DIR
`undef CY_BLK_DIR
`endif

`ifdef WARP
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\cy_virtualmux_v1_0"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\cy_virtualmux_v1_0\cy_virtualmux_v1_0.v"
`else
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\cy_virtualmux_v1_0"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\cy_virtualmux_v1_0\cy_virtualmux_v1_0.v"
`endif

// Component: B_UART_v2_50
`ifdef CY_BLK_DIR
`undef CY_BLK_DIR
`endif

`ifdef WARP
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\B_UART_v2_50"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\B_UART_v2_50\B_UART_v2_50.v"
`else
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\B_UART_v2_50"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\B_UART_v2_50\B_UART_v2_50.v"
`endif

// UART_v2_50(Address1=0, Address2=0, BaudRate=9600, BreakBitsRX=13, BreakBitsTX=13, BreakDetect=false, CRCoutputsEn=false, Enable_RX=1, Enable_RXIntInterrupt=1, Enable_TX=1, Enable_TXIntInterrupt=0, EnableHWAddress=0, EnIntRXInterrupt=true, EnIntTXInterrupt=false, FlowControl=0, HalfDuplexEn=false, HwTXEnSignal=false, InternalClock=true, InternalClockToleranceMinus=3.93736842105263, InternalClockTolerancePlus=3.93736842105263, InternalClockUsed=1, InterruptOnAddDetect=0, InterruptOnAddressMatch=0, InterruptOnBreak=0, InterruptOnByteRcvd=1, InterruptOnOverrunError=0, InterruptOnParityError=0, InterruptOnStopError=0, InterruptOnTXComplete=false, InterruptOnTXFifoEmpty=false, InterruptOnTXFifoFull=false, InterruptOnTXFifoNotFull=false, IntOnAddressDetect=false, IntOnAddressMatch=false, IntOnBreak=false, IntOnByteRcvd=true, IntOnOverrunError=false, IntOnParityError=false, IntOnStopError=false, NumDataBits=8, NumStopBits=1, OverSamplingRate=8, ParityType=0, ParityTypeSw=false, RequiredClock=76800, RXAddressMode=0, RXBufferSize=512, RxBuffRegSizeReplacementString=uint16, RXEnable=true, TXBitClkGenDP=true, TXBufferSize=4, TxBuffRegSizeReplacementString=uint8, TXEnable=true, Use23Polling=true, CY_API_CALLBACK_HEADER_INCLUDE=#include "cyapicallbacks.h", CY_COMPONENT_NAME=UART_v2_50, CY_CONTROL_FILE=<:default:>, CY_DATASHEET_FILE=<:default:>, CY_FITTER_NAME=RF_Physical, CY_INSTANCE_SHORT_NAME=RF_Physical, CY_MAJOR_VERSION=2, CY_MINOR_VERSION=50, CY_REMOVE=false, CY_SUPPRESS_API_GEN=false, CY_VERSION=PSoC Creator  3.3 SP1, INSTANCE_NAME=RF_Physical, )
module UART_v2_50_0 (
    rx_clk,
    rx_data,
    tx_clk,
    tx_data,
    rx_interrupt,
    tx_interrupt,
    tx,
    tx_en,
    rts_n,
    reset,
    cts_n,
    clock,
    rx);
    output      rx_clk;
    output      rx_data;
    output      tx_clk;
    output      tx_data;
    output      rx_interrupt;
    output      tx_interrupt;
    output      tx;
    output      tx_en;
    output      rts_n;
    input       reset;
    input       cts_n;
    input       clock;
    input       rx;

    parameter Address1 = 0;
    parameter Address2 = 0;
    parameter EnIntRXInterrupt = 1;
    parameter EnIntTXInterrupt = 0;
    parameter FlowControl = 0;
    parameter HalfDuplexEn = 0;
    parameter HwTXEnSignal = 0;
    parameter NumDataBits = 8;
    parameter NumStopBits = 1;
    parameter ParityType = 0;
    parameter RXEnable = 1;
    parameter TXEnable = 1;

          wire  Net_289;
          wire  Net_61;
          wire  Net_9;


	cy_clock_v1_0
		#(.id("b0162966-0060-4af5-82d1-fcb491ad7619/be0a0e37-ad17-42ca-b5a1-1a654d736358"),
		  .source_clock_id(""),
		  .divisor(0),
		  .period("13020833333.3333"),
		  .is_direct(0),
		  .is_digital(1))
		IntClock
		 (.clock_out(Net_9));



	cy_isr_v1_0
		#(.int_type(2'b10))
		RXInternalInterrupt
		 (.int_signal(rx_interrupt));


	// VirtualMux_1 (cy_virtualmux_v1_0)
	assign Net_61 = Net_9;

    B_UART_v2_50 BUART (
        .cts_n(cts_n),
        .tx(tx),
        .rts_n(rts_n),
        .tx_en(tx_en),
        .clock(Net_61),
        .reset(reset),
        .rx(rx),
        .tx_interrupt(tx_interrupt),
        .rx_interrupt(rx_interrupt),
        .tx_data(tx_data),
        .tx_clk(tx_clk),
        .rx_data(rx_data),
        .rx_clk(rx_clk));
    defparam BUART.Address1 = 0;
    defparam BUART.Address2 = 0;
    defparam BUART.BreakBitsRX = 13;
    defparam BUART.BreakBitsTX = 13;
    defparam BUART.BreakDetect = 0;
    defparam BUART.CRCoutputsEn = 0;
    defparam BUART.FlowControl = 0;
    defparam BUART.HalfDuplexEn = 0;
    defparam BUART.HwTXEnSignal = 0;
    defparam BUART.NumDataBits = 8;
    defparam BUART.NumStopBits = 1;
    defparam BUART.OverSampleCount = 8;
    defparam BUART.ParityType = 0;
    defparam BUART.ParityTypeSw = 0;
    defparam BUART.RXAddressMode = 0;
    defparam BUART.RXEnable = 1;
    defparam BUART.RXStatusIntEnable = 1;
    defparam BUART.TXBitClkGenDP = 1;
    defparam BUART.TXEnable = 1;
    defparam BUART.Use23Polling = 1;



endmodule

// Component: CyControlReg_v1_80
`ifdef CY_BLK_DIR
`undef CY_BLK_DIR
`endif

`ifdef WARP
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\CyControlReg_v1_80"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\CyControlReg_v1_80\CyControlReg_v1_80.v"
`else
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\CyControlReg_v1_80"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\CyControlReg_v1_80\CyControlReg_v1_80.v"
`endif

// Component: ZeroTerminal
`ifdef CY_BLK_DIR
`undef CY_BLK_DIR
`endif

`ifdef WARP
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\ZeroTerminal"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\ZeroTerminal\ZeroTerminal.v"
`else
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\ZeroTerminal"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\ZeroTerminal\ZeroTerminal.v"
`endif

// Component: B_Timer_v2_70
`ifdef CY_BLK_DIR
`undef CY_BLK_DIR
`endif

`ifdef WARP
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\B_Timer_v2_70"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\B_Timer_v2_70\B_Timer_v2_70.v"
`else
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\B_Timer_v2_70"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\B_Timer_v2_70\B_Timer_v2_70.v"
`endif

// Component: OneTerminal
`ifdef CY_BLK_DIR
`undef CY_BLK_DIR
`endif

`ifdef WARP
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\OneTerminal"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\OneTerminal\OneTerminal.v"
`else
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\OneTerminal"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\OneTerminal\OneTerminal.v"
`endif

// Timer_v2_70(CaptureAlternatingFall=false, CaptureAlternatingRise=false, CaptureCount=2, CaptureCounterEnabled=false, CaptureInputEnabled=false, CaptureMode=0, CONTROL3=0, ControlRegRemoved=0, CtlModeReplacementString=SyncCtl, CyGetRegReplacementString=CY_GET_REG16, CySetRegReplacementString=CY_SET_REG16, DeviceFamily=PSoC5, EnableMode=0, FF16=false, FF8=false, FixedFunction=false, FixedFunctionUsed=0, HWCaptureCounterEnabled=false, InterruptOnCapture=false, InterruptOnFIFOFull=false, InterruptOnTC=true, IntOnCapture=0, IntOnFIFOFull=0, IntOnTC=1, NumberOfCaptures=1, param45=1, Period=1499, RegDefReplacementString=reg16, RegSizeReplacementString=uint16, Resolution=16, RstStatusReplacementString=rstSts, RunMode=0, SiliconRevision=0, SoftwareCaptureModeEnabled=false, SoftwareTriggerModeEnabled=false, TriggerInputEnabled=false, TriggerMode=0, UDB16=true, UDB24=false, UDB32=false, UDB8=false, UDBControlReg=true, UsesHWEnable=0, VerilogSectionReplacementString=sT16, CY_API_CALLBACK_HEADER_INCLUDE=#include "cyapicallbacks.h", CY_COMPONENT_NAME=Timer_v2_70, CY_CONTROL_FILE=<:default:>, CY_DATASHEET_FILE=<:default:>, CY_FITTER_NAME=Timer_DispenserState, CY_INSTANCE_SHORT_NAME=Timer_DispenserState, CY_MAJOR_VERSION=2, CY_MINOR_VERSION=70, CY_REMOVE=false, CY_SUPPRESS_API_GEN=false, CY_VERSION=PSoC Creator  3.3 SP1, INSTANCE_NAME=Timer_DispenserState, )
module Timer_v2_70_1 (
    clock,
    reset,
    interrupt,
    enable,
    capture,
    trigger,
    capture_out,
    tc);
    input       clock;
    input       reset;
    output      interrupt;
    input       enable;
    input       capture;
    input       trigger;
    output      capture_out;
    output      tc;

    parameter CaptureCount = 2;
    parameter CaptureCounterEnabled = 0;
    parameter DeviceFamily = "PSoC5";
    parameter InterruptOnCapture = 0;
    parameter InterruptOnTC = 1;
    parameter Resolution = 16;
    parameter SiliconRevision = "0";

          wire  Net_261;
          wire  Net_260;
          wire  Net_266;
          wire  Net_102;
          wire  Net_55;
          wire  Net_57;
          wire  Net_53;
          wire  Net_51;

    ZeroTerminal ZeroTerminal_1 (
        .z(Net_260));

	// VirtualMux_2 (cy_virtualmux_v1_0)
	assign interrupt = Net_55;

	// VirtualMux_3 (cy_virtualmux_v1_0)
	assign tc = Net_53;

    B_Timer_v2_70 TimerUDB (
        .reset(reset),
        .interrupt(Net_55),
        .enable(enable),
        .trigger(trigger),
        .capture_in(capture),
        .capture_out(capture_out),
        .tc(Net_53),
        .clock(clock));
    defparam TimerUDB.Capture_Count = 2;
    defparam TimerUDB.CaptureCounterEnabled = 0;
    defparam TimerUDB.CaptureMode = 0;
    defparam TimerUDB.EnableMode = 0;
    defparam TimerUDB.InterruptOnCapture = 0;
    defparam TimerUDB.Resolution = 16;
    defparam TimerUDB.RunMode = 0;
    defparam TimerUDB.TriggerMode = 0;

    OneTerminal OneTerminal_1 (
        .o(Net_102));

	// VirtualMux_1 (cy_virtualmux_v1_0)
	assign Net_266 = Net_102;



endmodule

// Component: BShiftReg_v2_30
`ifdef CY_BLK_DIR
`undef CY_BLK_DIR
`endif

`ifdef WARP
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\BShiftReg_v2_30"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\BShiftReg_v2_30\BShiftReg_v2_30.v"
`else
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\BShiftReg_v2_30"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\BShiftReg_v2_30\BShiftReg_v2_30.v"
`endif

// ShiftReg_v2_30(ControlRegUsageReplacemetString=SyncCtl, CyGetRegReplacementString=CY_GET_REG8, CySetRegReplacementString=CY_SET_REG8, DefSi=0, Direction=1, FifoSize=4, InterruptSource=0, Length=4, RegDefReplacementString=reg8, RegSizeReplacementString=uint8, UseInputFifo=false, UseInterrupt=false, UseOutputFifo=false, UseShiftIn=true, UseShiftOut=true, VerilogSectionReplacementString=sC8, CY_API_CALLBACK_HEADER_INCLUDE=#include "cyapicallbacks.h", CY_COMPONENT_NAME=ShiftReg_v2_30, CY_CONTROL_FILE=<:default:>, CY_DATASHEET_FILE=<:default:>, CY_FITTER_NAME=ShiftReg_iButtonCRC4LSB, CY_INSTANCE_SHORT_NAME=ShiftReg_iButtonCRC4LSB, CY_MAJOR_VERSION=2, CY_MINOR_VERSION=30, CY_REMOVE=false, CY_SUPPRESS_API_GEN=false, CY_VERSION=PSoC Creator  3.3 SP1, INSTANCE_NAME=ShiftReg_iButtonCRC4LSB, )
module ShiftReg_v2_30_2 (
    shift_in,
    shift_out,
    load,
    store,
    clock,
    reset,
    interrupt);
    input       shift_in;
    output      shift_out;
    input       load;
    input       store;
    input       clock;
    input       reset;
    output      interrupt;

    parameter Direction = 1;
    parameter FifoSize = 4;
    parameter Length = 4;

          wire  Net_2;
          wire  Net_1;
          wire  Net_350;

	// VirtualMux_3 (cy_virtualmux_v1_0)
	assign Net_350 = shift_in;

    ZeroTerminal ZeroTerminal_3 (
        .z(Net_1));

    OneTerminal OneTerminal_1 (
        .o(Net_2));

    BShiftReg_v2_30 bSR (
        .shiftIn(Net_350),
        .load(load),
        .shiftOut(shift_out),
        .store(store),
        .clock(clock),
        .reset(reset),
        .interrupt(interrupt));
    defparam bSR.DefSi = 0;
    defparam bSR.Direction = 1;
    defparam bSR.FifoSize = 4;
    defparam bSR.InterruptSource = 0;
    defparam bSR.Length = 4;
    defparam bSR.UseInputFifo = 0;
    defparam bSR.UseInterrupt = 0;
    defparam bSR.UseOutputFifo = 0;



endmodule

// Component: CRC_v2_40
`ifdef CY_BLK_DIR
`undef CY_BLK_DIR
`endif

`ifdef WARP
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\CRC_v2_40"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\CRC_v2_40\CRC_v2_40.v"
`else
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\CRC_v2_40"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cycomponentlibrary\CyComponentLibrary.cylib\CRC_v2_40\CRC_v2_40.v"
`endif

// ShiftReg_v2_30(ControlRegUsageReplacemetString=SyncCtl, CyGetRegReplacementString=CY_GET_REG8, CySetRegReplacementString=CY_SET_REG8, DefSi=0, Direction=1, FifoSize=4, InterruptSource=0, Length=8, RegDefReplacementString=reg8, RegSizeReplacementString=uint8, UseInputFifo=false, UseInterrupt=false, UseOutputFifo=false, UseShiftIn=false, UseShiftOut=true, VerilogSectionReplacementString=sC8, CY_API_CALLBACK_HEADER_INCLUDE=#include "cyapicallbacks.h", CY_COMPONENT_NAME=ShiftReg_v2_30, CY_CONTROL_FILE=<:default:>, CY_DATASHEET_FILE=<:default:>, CY_FITTER_NAME=ShiftReg_Security, CY_INSTANCE_SHORT_NAME=ShiftReg_Security, CY_MAJOR_VERSION=2, CY_MINOR_VERSION=30, CY_REMOVE=false, CY_SUPPRESS_API_GEN=false, CY_VERSION=PSoC Creator  3.3 SP1, INSTANCE_NAME=ShiftReg_Security, )
module ShiftReg_v2_30_3 (
    shift_in,
    shift_out,
    load,
    store,
    clock,
    reset,
    interrupt);
    input       shift_in;
    output      shift_out;
    input       load;
    input       store;
    input       clock;
    input       reset;
    output      interrupt;

    parameter Direction = 1;
    parameter FifoSize = 4;
    parameter Length = 8;

          wire  Net_2;
          wire  Net_1;
          wire  Net_350;

	// VirtualMux_3 (cy_virtualmux_v1_0)
	assign Net_350 = Net_1;

    ZeroTerminal ZeroTerminal_3 (
        .z(Net_1));

    OneTerminal OneTerminal_1 (
        .o(Net_2));

    BShiftReg_v2_30 bSR (
        .shiftIn(Net_350),
        .load(load),
        .shiftOut(shift_out),
        .store(store),
        .clock(clock),
        .reset(reset),
        .interrupt(interrupt));
    defparam bSR.DefSi = 0;
    defparam bSR.Direction = 1;
    defparam bSR.FifoSize = 4;
    defparam bSR.InterruptSource = 0;
    defparam bSR.Length = 8;
    defparam bSR.UseInputFifo = 0;
    defparam bSR.UseInterrupt = 0;
    defparam bSR.UseOutputFifo = 0;



endmodule

// Component: cy_constant_v1_0
`ifdef CY_BLK_DIR
`undef CY_BLK_DIR
`endif

`ifdef WARP
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\cy_constant_v1_0"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\cy_constant_v1_0\cy_constant_v1_0.v"
`else
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\cy_constant_v1_0"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\cy_constant_v1_0\cy_constant_v1_0.v"
`endif

// Component: xor_v1_0
`ifdef CY_BLK_DIR
`undef CY_BLK_DIR
`endif

`ifdef WARP
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\xor_v1_0"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\xor_v1_0\xor_v1_0.v"
`else
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\xor_v1_0"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\xor_v1_0\xor_v1_0.v"
`endif

// UART_v2_50(Address1=0, Address2=0, BaudRate=9600, BreakBitsRX=13, BreakBitsTX=13, BreakDetect=false, CRCoutputsEn=false, Enable_RX=1, Enable_RXIntInterrupt=0, Enable_TX=1, Enable_TXIntInterrupt=0, EnableHWAddress=0, EnIntRXInterrupt=false, EnIntTXInterrupt=false, FlowControl=0, HalfDuplexEn=false, HwTXEnSignal=false, InternalClock=false, InternalClockToleranceMinus=3.93736842105263, InternalClockTolerancePlus=3.93736842105263, InternalClockUsed=0, InterruptOnAddDetect=0, InterruptOnAddressMatch=0, InterruptOnBreak=0, InterruptOnByteRcvd=1, InterruptOnOverrunError=0, InterruptOnParityError=0, InterruptOnStopError=0, InterruptOnTXComplete=false, InterruptOnTXFifoEmpty=false, InterruptOnTXFifoFull=false, InterruptOnTXFifoNotFull=false, IntOnAddressDetect=false, IntOnAddressMatch=false, IntOnBreak=false, IntOnByteRcvd=true, IntOnOverrunError=false, IntOnParityError=false, IntOnStopError=false, NumDataBits=8, NumStopBits=1, OverSamplingRate=8, ParityType=0, ParityTypeSw=false, RequiredClock=76800, RXAddressMode=0, RXBufferSize=4, RxBuffRegSizeReplacementString=uint8, RXEnable=true, TXBitClkGenDP=true, TXBufferSize=4, TxBuffRegSizeReplacementString=uint8, TXEnable=true, Use23Polling=true, CY_API_CALLBACK_HEADER_INCLUDE=#include "cyapicallbacks.h", CY_COMPONENT_NAME=UART_v2_50, CY_CONTROL_FILE=<:default:>, CY_DATASHEET_FILE=<:default:>, CY_FITTER_NAME=Printer, CY_INSTANCE_SHORT_NAME=Printer, CY_MAJOR_VERSION=2, CY_MINOR_VERSION=50, CY_REMOVE=false, CY_SUPPRESS_API_GEN=false, CY_VERSION=PSoC Creator  3.3 SP1, INSTANCE_NAME=Printer, )
module UART_v2_50_4 (
    rx_clk,
    rx_data,
    tx_clk,
    tx_data,
    rx_interrupt,
    tx_interrupt,
    tx,
    tx_en,
    rts_n,
    reset,
    cts_n,
    clock,
    rx);
    output      rx_clk;
    output      rx_data;
    output      tx_clk;
    output      tx_data;
    output      rx_interrupt;
    output      tx_interrupt;
    output      tx;
    output      tx_en;
    output      rts_n;
    input       reset;
    input       cts_n;
    input       clock;
    input       rx;

    parameter Address1 = 0;
    parameter Address2 = 0;
    parameter EnIntRXInterrupt = 0;
    parameter EnIntTXInterrupt = 0;
    parameter FlowControl = 0;
    parameter HalfDuplexEn = 0;
    parameter HwTXEnSignal = 0;
    parameter NumDataBits = 8;
    parameter NumStopBits = 1;
    parameter ParityType = 0;
    parameter RXEnable = 1;
    parameter TXEnable = 1;

          wire  Net_289;
          wire  Net_61;
          wire  Net_9;

	// VirtualMux_1 (cy_virtualmux_v1_0)
	assign Net_61 = clock;

    B_UART_v2_50 BUART (
        .cts_n(cts_n),
        .tx(tx),
        .rts_n(rts_n),
        .tx_en(tx_en),
        .clock(Net_61),
        .reset(reset),
        .rx(rx),
        .tx_interrupt(tx_interrupt),
        .rx_interrupt(rx_interrupt),
        .tx_data(tx_data),
        .tx_clk(tx_clk),
        .rx_data(rx_data),
        .rx_clk(rx_clk));
    defparam BUART.Address1 = 0;
    defparam BUART.Address2 = 0;
    defparam BUART.BreakBitsRX = 13;
    defparam BUART.BreakBitsTX = 13;
    defparam BUART.BreakDetect = 0;
    defparam BUART.CRCoutputsEn = 0;
    defparam BUART.FlowControl = 0;
    defparam BUART.HalfDuplexEn = 0;
    defparam BUART.HwTXEnSignal = 0;
    defparam BUART.NumDataBits = 8;
    defparam BUART.NumStopBits = 1;
    defparam BUART.OverSampleCount = 8;
    defparam BUART.ParityType = 0;
    defparam BUART.ParityTypeSw = 0;
    defparam BUART.RXAddressMode = 0;
    defparam BUART.RXEnable = 1;
    defparam BUART.RXStatusIntEnable = 1;
    defparam BUART.TXBitClkGenDP = 1;
    defparam BUART.TXEnable = 1;
    defparam BUART.Use23Polling = 1;



endmodule

// UART_v2_50(Address1=0, Address2=0, BaudRate=4800, BreakBitsRX=13, BreakBitsTX=13, BreakDetect=false, CRCoutputsEn=false, Enable_RX=1, Enable_RXIntInterrupt=1, Enable_TX=1, Enable_TXIntInterrupt=0, EnableHWAddress=0, EnIntRXInterrupt=true, EnIntTXInterrupt=false, FlowControl=0, HalfDuplexEn=false, HwTXEnSignal=false, InternalClock=false, InternalClockToleranceMinus=3.93736842105263, InternalClockTolerancePlus=3.93736842105263, InternalClockUsed=0, InterruptOnAddDetect=0, InterruptOnAddressMatch=0, InterruptOnBreak=0, InterruptOnByteRcvd=1, InterruptOnOverrunError=0, InterruptOnParityError=0, InterruptOnStopError=0, InterruptOnTXComplete=false, InterruptOnTXFifoEmpty=false, InterruptOnTXFifoFull=false, InterruptOnTXFifoNotFull=false, IntOnAddressDetect=false, IntOnAddressMatch=false, IntOnBreak=false, IntOnByteRcvd=true, IntOnOverrunError=false, IntOnParityError=false, IntOnStopError=false, NumDataBits=8, NumStopBits=1, OverSamplingRate=8, ParityType=1, ParityTypeSw=false, RequiredClock=38400, RXAddressMode=0, RXBufferSize=256, RxBuffRegSizeReplacementString=uint16, RXEnable=true, TXBitClkGenDP=true, TXBufferSize=4, TxBuffRegSizeReplacementString=uint8, TXEnable=true, Use23Polling=true, CY_API_CALLBACK_HEADER_INCLUDE=#include "cyapicallbacks.h", CY_COMPONENT_NAME=UART_v2_50, CY_CONTROL_FILE=<:default:>, CY_DATASHEET_FILE=<:default:>, CY_FITTER_NAME=Dispenser, CY_INSTANCE_SHORT_NAME=Dispenser, CY_MAJOR_VERSION=2, CY_MINOR_VERSION=50, CY_REMOVE=false, CY_SUPPRESS_API_GEN=false, CY_VERSION=PSoC Creator  3.3 SP1, INSTANCE_NAME=Dispenser, )
module UART_v2_50_5 (
    rx_clk,
    rx_data,
    tx_clk,
    tx_data,
    rx_interrupt,
    tx_interrupt,
    tx,
    tx_en,
    rts_n,
    reset,
    cts_n,
    clock,
    rx);
    output      rx_clk;
    output      rx_data;
    output      tx_clk;
    output      tx_data;
    output      rx_interrupt;
    output      tx_interrupt;
    output      tx;
    output      tx_en;
    output      rts_n;
    input       reset;
    input       cts_n;
    input       clock;
    input       rx;

    parameter Address1 = 0;
    parameter Address2 = 0;
    parameter EnIntRXInterrupt = 1;
    parameter EnIntTXInterrupt = 0;
    parameter FlowControl = 0;
    parameter HalfDuplexEn = 0;
    parameter HwTXEnSignal = 0;
    parameter NumDataBits = 8;
    parameter NumStopBits = 1;
    parameter ParityType = 1;
    parameter RXEnable = 1;
    parameter TXEnable = 1;

          wire  Net_289;
          wire  Net_61;
          wire  Net_9;


	cy_isr_v1_0
		#(.int_type(2'b10))
		RXInternalInterrupt
		 (.int_signal(rx_interrupt));


	// VirtualMux_1 (cy_virtualmux_v1_0)
	assign Net_61 = clock;

    B_UART_v2_50 BUART (
        .cts_n(cts_n),
        .tx(tx),
        .rts_n(rts_n),
        .tx_en(tx_en),
        .clock(Net_61),
        .reset(reset),
        .rx(rx),
        .tx_interrupt(tx_interrupt),
        .rx_interrupt(rx_interrupt),
        .tx_data(tx_data),
        .tx_clk(tx_clk),
        .rx_data(rx_data),
        .rx_clk(rx_clk));
    defparam BUART.Address1 = 0;
    defparam BUART.Address2 = 0;
    defparam BUART.BreakBitsRX = 13;
    defparam BUART.BreakBitsTX = 13;
    defparam BUART.BreakDetect = 0;
    defparam BUART.CRCoutputsEn = 0;
    defparam BUART.FlowControl = 0;
    defparam BUART.HalfDuplexEn = 0;
    defparam BUART.HwTXEnSignal = 0;
    defparam BUART.NumDataBits = 8;
    defparam BUART.NumStopBits = 1;
    defparam BUART.OverSampleCount = 8;
    defparam BUART.ParityType = 1;
    defparam BUART.ParityTypeSw = 0;
    defparam BUART.RXAddressMode = 0;
    defparam BUART.RXEnable = 1;
    defparam BUART.RXStatusIntEnable = 1;
    defparam BUART.TXBitClkGenDP = 1;
    defparam BUART.TXEnable = 1;
    defparam BUART.Use23Polling = 1;



endmodule

// UART_v2_50(Address1=0, Address2=0, BaudRate=57600, BreakBitsRX=13, BreakBitsTX=13, BreakDetect=false, CRCoutputsEn=false, Enable_RX=1, Enable_RXIntInterrupt=1, Enable_TX=1, Enable_TXIntInterrupt=0, EnableHWAddress=0, EnIntRXInterrupt=true, EnIntTXInterrupt=false, FlowControl=0, HalfDuplexEn=false, HwTXEnSignal=false, InternalClock=true, InternalClockToleranceMinus=3.93736842105263, InternalClockTolerancePlus=3.93736842105263, InternalClockUsed=1, InterruptOnAddDetect=0, InterruptOnAddressMatch=0, InterruptOnBreak=0, InterruptOnByteRcvd=1, InterruptOnOverrunError=0, InterruptOnParityError=0, InterruptOnStopError=0, InterruptOnTXComplete=false, InterruptOnTXFifoEmpty=false, InterruptOnTXFifoFull=false, InterruptOnTXFifoNotFull=false, IntOnAddressDetect=false, IntOnAddressMatch=false, IntOnBreak=false, IntOnByteRcvd=true, IntOnOverrunError=false, IntOnParityError=false, IntOnStopError=false, NumDataBits=8, NumStopBits=1, OverSamplingRate=8, ParityType=0, ParityTypeSw=false, RequiredClock=460800, RXAddressMode=0, RXBufferSize=32, RxBuffRegSizeReplacementString=uint8, RXEnable=true, TXBitClkGenDP=true, TXBufferSize=4, TxBuffRegSizeReplacementString=uint8, TXEnable=true, Use23Polling=true, CY_API_CALLBACK_HEADER_INCLUDE=#include "cyapicallbacks.h", CY_COMPONENT_NAME=UART_v2_50, CY_CONTROL_FILE=<:default:>, CY_DATASHEET_FILE=<:default:>, CY_FITTER_NAME=Display1, CY_INSTANCE_SHORT_NAME=Display1, CY_MAJOR_VERSION=2, CY_MINOR_VERSION=50, CY_REMOVE=false, CY_SUPPRESS_API_GEN=false, CY_VERSION=PSoC Creator  3.3 SP1, INSTANCE_NAME=Display1, )
module UART_v2_50_6 (
    rx_clk,
    rx_data,
    tx_clk,
    tx_data,
    rx_interrupt,
    tx_interrupt,
    tx,
    tx_en,
    rts_n,
    reset,
    cts_n,
    clock,
    rx);
    output      rx_clk;
    output      rx_data;
    output      tx_clk;
    output      tx_data;
    output      rx_interrupt;
    output      tx_interrupt;
    output      tx;
    output      tx_en;
    output      rts_n;
    input       reset;
    input       cts_n;
    input       clock;
    input       rx;

    parameter Address1 = 0;
    parameter Address2 = 0;
    parameter EnIntRXInterrupt = 1;
    parameter EnIntTXInterrupt = 0;
    parameter FlowControl = 0;
    parameter HalfDuplexEn = 0;
    parameter HwTXEnSignal = 0;
    parameter NumDataBits = 8;
    parameter NumStopBits = 1;
    parameter ParityType = 0;
    parameter RXEnable = 1;
    parameter TXEnable = 1;

          wire  Net_289;
          wire  Net_61;
          wire  Net_9;


	cy_clock_v1_0
		#(.id("f231f45c-3a9d-47b3-8b64-43477054a10e/be0a0e37-ad17-42ca-b5a1-1a654d736358"),
		  .source_clock_id(""),
		  .divisor(0),
		  .period("2170138888.88889"),
		  .is_direct(0),
		  .is_digital(1))
		IntClock
		 (.clock_out(Net_9));



	cy_isr_v1_0
		#(.int_type(2'b10))
		RXInternalInterrupt
		 (.int_signal(rx_interrupt));


	// VirtualMux_1 (cy_virtualmux_v1_0)
	assign Net_61 = Net_9;

    B_UART_v2_50 BUART (
        .cts_n(cts_n),
        .tx(tx),
        .rts_n(rts_n),
        .tx_en(tx_en),
        .clock(Net_61),
        .reset(reset),
        .rx(rx),
        .tx_interrupt(tx_interrupt),
        .rx_interrupt(rx_interrupt),
        .tx_data(tx_data),
        .tx_clk(tx_clk),
        .rx_data(rx_data),
        .rx_clk(rx_clk));
    defparam BUART.Address1 = 0;
    defparam BUART.Address2 = 0;
    defparam BUART.BreakBitsRX = 13;
    defparam BUART.BreakBitsTX = 13;
    defparam BUART.BreakDetect = 0;
    defparam BUART.CRCoutputsEn = 0;
    defparam BUART.FlowControl = 0;
    defparam BUART.HalfDuplexEn = 0;
    defparam BUART.HwTXEnSignal = 0;
    defparam BUART.NumDataBits = 8;
    defparam BUART.NumStopBits = 1;
    defparam BUART.OverSampleCount = 8;
    defparam BUART.ParityType = 0;
    defparam BUART.ParityTypeSw = 0;
    defparam BUART.RXAddressMode = 0;
    defparam BUART.RXEnable = 1;
    defparam BUART.RXStatusIntEnable = 1;
    defparam BUART.TXBitClkGenDP = 1;
    defparam BUART.TXEnable = 1;
    defparam BUART.Use23Polling = 1;



endmodule

// UART_v2_50(Address1=0, Address2=0, BaudRate=57600, BreakBitsRX=13, BreakBitsTX=13, BreakDetect=false, CRCoutputsEn=false, Enable_RX=1, Enable_RXIntInterrupt=1, Enable_TX=1, Enable_TXIntInterrupt=0, EnableHWAddress=0, EnIntRXInterrupt=true, EnIntTXInterrupt=false, FlowControl=0, HalfDuplexEn=false, HwTXEnSignal=false, InternalClock=true, InternalClockToleranceMinus=3.93736842105263, InternalClockTolerancePlus=3.93736842105263, InternalClockUsed=1, InterruptOnAddDetect=0, InterruptOnAddressMatch=0, InterruptOnBreak=0, InterruptOnByteRcvd=1, InterruptOnOverrunError=0, InterruptOnParityError=0, InterruptOnStopError=0, InterruptOnTXComplete=false, InterruptOnTXFifoEmpty=false, InterruptOnTXFifoFull=false, InterruptOnTXFifoNotFull=false, IntOnAddressDetect=false, IntOnAddressMatch=false, IntOnBreak=false, IntOnByteRcvd=true, IntOnOverrunError=false, IntOnParityError=false, IntOnStopError=false, NumDataBits=8, NumStopBits=1, OverSamplingRate=8, ParityType=0, ParityTypeSw=false, RequiredClock=460800, RXAddressMode=0, RXBufferSize=32, RxBuffRegSizeReplacementString=uint8, RXEnable=true, TXBitClkGenDP=true, TXBufferSize=4, TxBuffRegSizeReplacementString=uint8, TXEnable=true, Use23Polling=true, CY_API_CALLBACK_HEADER_INCLUDE=#include "cyapicallbacks.h", CY_COMPONENT_NAME=UART_v2_50, CY_CONTROL_FILE=<:default:>, CY_DATASHEET_FILE=<:default:>, CY_FITTER_NAME=Display2, CY_INSTANCE_SHORT_NAME=Display2, CY_MAJOR_VERSION=2, CY_MINOR_VERSION=50, CY_REMOVE=false, CY_SUPPRESS_API_GEN=false, CY_VERSION=PSoC Creator  3.3 SP1, INSTANCE_NAME=Display2, )
module UART_v2_50_7 (
    rx_clk,
    rx_data,
    tx_clk,
    tx_data,
    rx_interrupt,
    tx_interrupt,
    tx,
    tx_en,
    rts_n,
    reset,
    cts_n,
    clock,
    rx);
    output      rx_clk;
    output      rx_data;
    output      tx_clk;
    output      tx_data;
    output      rx_interrupt;
    output      tx_interrupt;
    output      tx;
    output      tx_en;
    output      rts_n;
    input       reset;
    input       cts_n;
    input       clock;
    input       rx;

    parameter Address1 = 0;
    parameter Address2 = 0;
    parameter EnIntRXInterrupt = 1;
    parameter EnIntTXInterrupt = 0;
    parameter FlowControl = 0;
    parameter HalfDuplexEn = 0;
    parameter HwTXEnSignal = 0;
    parameter NumDataBits = 8;
    parameter NumStopBits = 1;
    parameter ParityType = 0;
    parameter RXEnable = 1;
    parameter TXEnable = 1;

          wire  Net_289;
          wire  Net_61;
          wire  Net_9;


	cy_clock_v1_0
		#(.id("7f85b586-7c4f-4060-9221-20a5f6f31e8c/be0a0e37-ad17-42ca-b5a1-1a654d736358"),
		  .source_clock_id(""),
		  .divisor(0),
		  .period("2170138888.88889"),
		  .is_direct(0),
		  .is_digital(1))
		IntClock
		 (.clock_out(Net_9));



	cy_isr_v1_0
		#(.int_type(2'b10))
		RXInternalInterrupt
		 (.int_signal(rx_interrupt));


	// VirtualMux_1 (cy_virtualmux_v1_0)
	assign Net_61 = Net_9;

    B_UART_v2_50 BUART (
        .cts_n(cts_n),
        .tx(tx),
        .rts_n(rts_n),
        .tx_en(tx_en),
        .clock(Net_61),
        .reset(reset),
        .rx(rx),
        .tx_interrupt(tx_interrupt),
        .rx_interrupt(rx_interrupt),
        .tx_data(tx_data),
        .tx_clk(tx_clk),
        .rx_data(rx_data),
        .rx_clk(rx_clk));
    defparam BUART.Address1 = 0;
    defparam BUART.Address2 = 0;
    defparam BUART.BreakBitsRX = 13;
    defparam BUART.BreakBitsTX = 13;
    defparam BUART.BreakDetect = 0;
    defparam BUART.CRCoutputsEn = 0;
    defparam BUART.FlowControl = 0;
    defparam BUART.HalfDuplexEn = 0;
    defparam BUART.HwTXEnSignal = 0;
    defparam BUART.NumDataBits = 8;
    defparam BUART.NumStopBits = 1;
    defparam BUART.OverSampleCount = 8;
    defparam BUART.ParityType = 0;
    defparam BUART.ParityTypeSw = 0;
    defparam BUART.RXAddressMode = 0;
    defparam BUART.RXEnable = 1;
    defparam BUART.RXStatusIntEnable = 1;
    defparam BUART.TXBitClkGenDP = 1;
    defparam BUART.TXEnable = 1;
    defparam BUART.Use23Polling = 1;



endmodule

// Component: or_v1_0
`ifdef CY_BLK_DIR
`undef CY_BLK_DIR
`endif

`ifdef WARP
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\or_v1_0"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\or_v1_0\or_v1_0.v"
`else
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\or_v1_0"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\or_v1_0\or_v1_0.v"
`endif

// I2C_v3_50(Address_Decode=1, BusSpeed_kHz=100, ClockInputVisibility=false, CtlModeReplacementString=SyncCtl, EnableWakeup=false, ExternalBuffer=false, Externi2cIntrHandler=false, ExternTmoutIntrHandler=false, FF=true, Hex=false, I2C_Mode=2, I2cBusPort=0, Implementation=1, InternalUdbClockToleranceMinus=5, InternalUdbClockTolerancePlus=5, NotSlaveClockMinusTolerance=5, NotSlaveClockPlusTolerance=5, PrescalerEnabled=false, PrescalerPeriod=3, Psoc3ffSelected=false, Psoc5AffSelected=false, Psoc5lpffSelected=true, RemoveI2cff=false, RemoveI2cUdb=true, RemoveIntClock=true, RemoveTimeoutTimer=true, SclTimeoutEnabled=false, SdaTimeoutEnabled=false, Slave_Address=8, SlaveClockMinusTolerance=5, SlaveClockPlusTolerance=50, TimeoutEnabled=false, TimeoutImplementation=0, TimeOutms=25, TimeoutPeriodff=39999, TimeoutPeriodUdb=39999, UDB_MSTR=false, UDB_MULTI_MASTER_SLAVE=false, UDB_SLV=false, UdbInternalClock=true, UdbRequiredClock=1600, UdbSlaveFixedPlacementEnable=false, CY_API_CALLBACK_HEADER_INCLUDE=#include "cyapicallbacks.h", CY_COMPONENT_NAME=I2C_v3_50, CY_CONTROL_FILE=I2C_Slave_DefaultPlacement.ctl, CY_DATASHEET_FILE=<:default:>, CY_FITTER_NAME=I2C_Bus, CY_INSTANCE_SHORT_NAME=I2C_Bus, CY_MAJOR_VERSION=3, CY_MINOR_VERSION=50, CY_REMOVE=false, CY_SUPPRESS_API_GEN=false, CY_VERSION=PSoC Creator  3.3 SP1, INSTANCE_NAME=I2C_Bus, )
module I2C_v3_50_8 (
    sda_o,
    scl_o,
    sda_i,
    scl_i,
    iclk,
    bclk,
    reset,
    clock,
    scl,
    sda,
    itclk);
    output      sda_o;
    output      scl_o;
    input       sda_i;
    input       scl_i;
    output      iclk;
    output      bclk;
    input       reset;
    input       clock;
    inout       scl;
    inout       sda;
    output      itclk;


          wire  sda_x_wire;
          wire  sda_yfb;
          wire  udb_clk;
          wire  Net_975;
          wire  Net_974;
          wire  Net_973;
          wire  bus_clk;
          wire  Net_972;
          wire  Net_968;
          wire  scl_yfb;
          wire  Net_969;
          wire  Net_971;
          wire  Net_970;
          wire  timeout_clk;
          wire  Net_697;
          wire  Net_1045;
          wire [1:0] Net_1109;
          wire [5:0] Net_643;
          wire  scl_x_wire;

	// Vmux_sda_out (cy_virtualmux_v1_0)
	assign sda_x_wire = Net_643[1];


	cy_isr_v1_0
		#(.int_type(2'b00))
		I2C_IRQ
		 (.int_signal(Net_697));


    cy_psoc3_i2c_v1_0 I2C_FF (
        .clock(bus_clk),
        .scl_in(Net_1109[0]),
        .sda_in(Net_1109[1]),
        .scl_out(Net_643[0]),
        .sda_out(Net_643[1]),
        .interrupt(Net_643[2]));
    defparam I2C_FF.use_wakeup = 0;

	// Vmux_interrupt (cy_virtualmux_v1_0)
	assign Net_697 = Net_643[2];

	// Vmux_scl_out (cy_virtualmux_v1_0)
	assign scl_x_wire = Net_643[0];

    OneTerminal OneTerminal_1 (
        .o(Net_969));

    OneTerminal OneTerminal_2 (
        .o(Net_968));

	// Vmux_clock (cy_virtualmux_v1_0)
	assign udb_clk = Net_970;


	cy_clock_v1_0
		#(.id("6f2d57bd-b6d0-4115-93da-ded3485bf4ed/5ece924d-20ba-480e-9102-bc082dcdd926"),
		  .source_clock_id("75C2148C-3656-4d8a-846D-0CAE99AB6FF7"),
		  .divisor(0),
		  .period("0"),
		  .is_direct(1),
		  .is_digital(1))
		BusClock
		 (.clock_out(bus_clk));



    assign bclk = bus_clk | Net_973;

    ZeroTerminal ZeroTerminal_1 (
        .z(Net_973));


    assign iclk = udb_clk | Net_974;

    ZeroTerminal ZeroTerminal_2 (
        .z(Net_974));

	// Vmux_scl_in (cy_virtualmux_v1_0)
	assign Net_1109[0] = scl_yfb;

	// Vmux_sda_in (cy_virtualmux_v1_0)
	assign Net_1109[1] = sda_yfb;

	wire [0:0] tmpOE__Bufoe_scl_net;

	cy_bufoe
		Bufoe_scl
		 (.x(scl_x_wire),
		  .y(scl),
		  .oe(tmpOE__Bufoe_scl_net),
		  .yfb(scl_yfb));

	assign tmpOE__Bufoe_scl_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{Net_969} : {Net_969};

	wire [0:0] tmpOE__Bufoe_sda_net;

	cy_bufoe
		Bufoe_sda
		 (.x(sda_x_wire),
		  .y(sda),
		  .oe(tmpOE__Bufoe_sda_net),
		  .yfb(sda_yfb));

	assign tmpOE__Bufoe_sda_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{Net_968} : {Net_968};

	// Vmux_timeout_clock (cy_virtualmux_v1_0)
	assign timeout_clk = clock;


    assign itclk = timeout_clk | Net_975;

    ZeroTerminal ZeroTerminal_3 (
        .z(Net_975));


    assign sda_o = sda_x_wire;

    assign scl_o = scl_x_wire;


endmodule

// Timer_v2_70(CaptureAlternatingFall=false, CaptureAlternatingRise=false, CaptureCount=2, CaptureCounterEnabled=false, CaptureInputEnabled=false, CaptureMode=0, CONTROL3=1, ControlRegRemoved=0, CtlModeReplacementString=SyncCtl, CyGetRegReplacementString=CY_GET_REG16, CySetRegReplacementString=CY_SET_REG16, DeviceFamily=PSoC5, EnableMode=0, FF16=true, FF8=false, FixedFunction=true, FixedFunctionUsed=1, HWCaptureCounterEnabled=false, InterruptOnCapture=false, InterruptOnFIFOFull=false, InterruptOnTC=true, IntOnCapture=0, IntOnFIFOFull=0, IntOnTC=1, NumberOfCaptures=1, param45=1, Period=999, RegDefReplacementString=reg16, RegSizeReplacementString=uint16, Resolution=16, RstStatusReplacementString=rstSts, RunMode=0, SiliconRevision=0, SoftwareCaptureModeEnabled=false, SoftwareTriggerModeEnabled=false, TriggerInputEnabled=false, TriggerMode=0, UDB16=false, UDB24=false, UDB32=false, UDB8=false, UDBControlReg=false, UsesHWEnable=0, VerilogSectionReplacementString=sT16, CY_API_CALLBACK_HEADER_INCLUDE=#include "cyapicallbacks.h", CY_COMPONENT_NAME=Timer_v2_70, CY_CONTROL_FILE=<:default:>, CY_DATASHEET_FILE=<:default:>, CY_FITTER_NAME=Timer_Tick, CY_INSTANCE_SHORT_NAME=Timer_Tick, CY_MAJOR_VERSION=2, CY_MINOR_VERSION=70, CY_REMOVE=false, CY_SUPPRESS_API_GEN=false, CY_VERSION=PSoC Creator  3.3 SP1, INSTANCE_NAME=Timer_Tick, )
module Timer_v2_70_9 (
    clock,
    reset,
    interrupt,
    enable,
    capture,
    trigger,
    capture_out,
    tc);
    input       clock;
    input       reset;
    output      interrupt;
    input       enable;
    input       capture;
    input       trigger;
    output      capture_out;
    output      tc;

    parameter CaptureCount = 2;
    parameter CaptureCounterEnabled = 0;
    parameter DeviceFamily = "PSoC5";
    parameter InterruptOnCapture = 0;
    parameter InterruptOnTC = 1;
    parameter Resolution = 16;
    parameter SiliconRevision = "0";

          wire  Net_261;
          wire  Net_260;
          wire  Net_266;
          wire  Net_102;
          wire  Net_55;
          wire  Net_57;
          wire  Net_53;
          wire  Net_51;

    cy_psoc3_timer_v1_0 TimerHW (
        .timer_reset(reset),
        .capture(capture),
        .enable(Net_266),
        .kill(Net_260),
        .clock(clock),
        .tc(Net_51),
        .compare(Net_261),
        .interrupt(Net_57));

    ZeroTerminal ZeroTerminal_1 (
        .z(Net_260));

	// VirtualMux_2 (cy_virtualmux_v1_0)
	assign interrupt = Net_57;

	// VirtualMux_3 (cy_virtualmux_v1_0)
	assign tc = Net_51;

    OneTerminal OneTerminal_1 (
        .o(Net_102));

	// VirtualMux_1 (cy_virtualmux_v1_0)
	assign Net_266 = Net_102;



endmodule

// ShiftReg_v2_30(ControlRegUsageReplacemetString=SyncCtl, CyGetRegReplacementString=CY_GET_REG8, CySetRegReplacementString=CY_SET_REG8, DefSi=0, Direction=1, FifoSize=4, InterruptSource=0, Length=3, RegDefReplacementString=reg8, RegSizeReplacementString=uint8, UseInputFifo=false, UseInterrupt=false, UseOutputFifo=false, UseShiftIn=true, UseShiftOut=true, VerilogSectionReplacementString=sC8, CY_API_CALLBACK_HEADER_INCLUDE=#include "cyapicallbacks.h", CY_COMPONENT_NAME=ShiftReg_v2_30, CY_CONTROL_FILE=<:default:>, CY_DATASHEET_FILE=<:default:>, CY_FITTER_NAME=ShiftReg_iButtonCRC3MSB, CY_INSTANCE_SHORT_NAME=ShiftReg_iButtonCRC3MSB, CY_MAJOR_VERSION=2, CY_MINOR_VERSION=30, CY_REMOVE=false, CY_SUPPRESS_API_GEN=false, CY_VERSION=PSoC Creator  3.3 SP1, INSTANCE_NAME=ShiftReg_iButtonCRC3MSB, )
module ShiftReg_v2_30_10 (
    shift_in,
    shift_out,
    load,
    store,
    clock,
    reset,
    interrupt);
    input       shift_in;
    output      shift_out;
    input       load;
    input       store;
    input       clock;
    input       reset;
    output      interrupt;

    parameter Direction = 1;
    parameter FifoSize = 4;
    parameter Length = 3;

          wire  Net_2;
          wire  Net_1;
          wire  Net_350;

	// VirtualMux_3 (cy_virtualmux_v1_0)
	assign Net_350 = shift_in;

    ZeroTerminal ZeroTerminal_3 (
        .z(Net_1));

    OneTerminal OneTerminal_1 (
        .o(Net_2));

    BShiftReg_v2_30 bSR (
        .shiftIn(Net_350),
        .load(load),
        .shiftOut(shift_out),
        .store(store),
        .clock(clock),
        .reset(reset),
        .interrupt(interrupt));
    defparam bSR.DefSi = 0;
    defparam bSR.Direction = 1;
    defparam bSR.FifoSize = 4;
    defparam bSR.InterruptSource = 0;
    defparam bSR.Length = 3;
    defparam bSR.UseInputFifo = 0;
    defparam bSR.UseInterrupt = 0;
    defparam bSR.UseOutputFifo = 0;



endmodule

// Component: CyStatusReg_v1_90
`ifdef CY_BLK_DIR
`undef CY_BLK_DIR
`endif

`ifdef WARP
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\CyStatusReg_v1_90"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\CyStatusReg_v1_90\CyStatusReg_v1_90.v"
`else
`define CY_BLK_DIR "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\CyStatusReg_v1_90"
`include "C:\Program Files (x86)\Cypress\PSoC Creator\3.3\PSoC Creator\psoc\content\cyprimitives\CyPrimitives.cylib\CyStatusReg_v1_90\CyStatusReg_v1_90.v"
`endif

// top
module top ;

          wire  Net_1042;
          wire [7:0] Net_992;
          wire  Net_991;
          wire  Net_990;
          wire  Net_989;
          wire  Net_988;
          wire  Net_987;
          wire  Net_985;
          wire  Net_984;
          wire  Net_983;
          wire  Net_1004;
          wire  Net_1003;
          wire  Net_1002;
          wire  Net_816;
          wire  Net_815;
          wire  Net_814;
          wire  Net_813;
          wire  Net_812;
          wire  Net_805;
          wire  Net_1008;
          wire  Net_1007;
          wire  Net_664;
          wire  Net_663;
          wire  Net_662;
          wire  Net_661;
          wire  Net_660;
          wire  Net_659;
          wire  Net_658;
          wire  Net_657;
          wire  Net_656;
          wire  Net_626;
          wire  Net_625;
          wire  Net_532;
          wire  Net_531;
          wire  Net_530;
          wire  Net_529;
          wire  Net_528;
          wire  Net_527;
          wire  Net_504;
          wire  Net_1045;
          wire  Net_524;
          wire  Net_523;
          wire  Net_499;
          wire  Net_522;
          wire  Net_496;
          wire  Net_495;
          wire  Net_494;
          wire  Net_493;
          wire  Net_492;
          wire  Net_491;
          wire  Net_449;
          wire  Net_489;
          wire  Net_488;
          wire  Net_487;
          wire  Net_444;
          wire  Net_486;
          wire  Net_441;
          wire  Net_485;
          wire  Net_484;
          wire  Net_483;
          wire  Net_482;
          wire  Net_481;
          wire  Net_480;
          wire  Net_397;
          wire  Net_479;
          wire  Net_478;
          wire  Net_392;
          wire  Net_477;
          wire  Net_476;
          wire  Net_475;
          wire  Net_474;
          wire  Net_473;
          wire  Net_472;
          wire  Net_471;
          wire  Net_363;
          wire  Net_469;
          wire  Net_468;
          wire  Net_358;
          wire  Net_467;
          wire  Net_336;
          wire  Net_335;
          wire  Net_334;
          wire  Net_333;
          wire  Net_332;
          wire  Net_331;
          wire  Net_329;
          wire  Net_328;
          wire  Net_327;
          wire  Net_868;
          wire  Net_295;
          wire  Net_294;
          wire  Net_293;
          wire  Net_292;
          wire  Net_291;
          wire  Net_290;
          wire  Net_289;
          wire  Net_288;
          wire  Net_287;
          wire  Net_264;
          wire  Net_263;
          wire  Net_262;
          wire  Net_261;
          wire  Net_260;
          wire  Net_259;
          wire  Net_257;
          wire  Net_256;
          wire  Net_255;
          wire  Net_166;
          wire  Net_234;
          wire  Net_165;
          wire  Net_164;
          wire  Net_163;
          wire  Net_213;
          wire  Net_212;
          wire  Net_211;
          wire  Net_210;
          wire  Net_209;
          wire  Net_208;
          wire  Net_206;
          wire  Net_205;
          wire  Net_204;
          wire  Net_892;
          wire  Net_891;
          wire  Net_890;
          wire  Net_889;
          wire  Net_888;
          wire  Net_887;
          wire  Net_886;
          wire  Net_885;
          wire  Net_884;
          wire  Net_162;
          wire  Net_161;
          wire  Net_1001;
          wire  Net_858;
          wire  Net_857;
          wire  Net_856;
          wire  Net_855;
          wire  Net_854;
          wire  Net_867;
          wire  Net_866;
          wire  Net_865;
          wire  Net_864;
          wire  Net_863;
          wire  Net_862;
          wire  Net_861;
          wire  Net_860;
          wire  Net_859;
          wire  Net_35;
          wire  Net_34;
          wire  Net_33;
          wire  Net_32;
          wire  Net_36;
          wire  Net_30;
          wire  Net_7;
          wire  Net_1047;
          wire  Net_27;
          wire  Net_26;
          wire  Net_2;
          wire  Net_25;
          wire  Net_1067;
          wire  Net_470;
          wire  Net_160;
          wire  Net_997;
          wire  Net_950;
          wire  Net_152;
          wire  Net_996;
          wire  Net_872;
          wire  Net_87;
          wire  Net_995;
          wire  Net_59;
          wire  Net_10;
          wire  Net_12;
          wire  Net_734;
          wire  Net_701;
          wire  Net_736;
          wire  Net_526;
          wire  Net_490;
          wire  Net_429;
          wire  Net_362;
          wire  Net_29;
          wire  Net_140;
          wire  Net_111;
          wire  Net_133;
          wire  Net_113;

    UART_v2_50_0 RF_Physical (
        .cts_n(1'b0),
        .tx(Net_2),
        .rts_n(Net_26),
        .tx_en(Net_27),
        .clock(1'b0),
        .reset(Net_29),
        .rx(Net_7),
        .tx_interrupt(Net_30),
        .rx_interrupt(Net_36),
        .tx_data(Net_32),
        .tx_clk(Net_33),
        .rx_data(Net_34),
        .rx_clk(Net_35));
    defparam RF_Physical.Address1 = 0;
    defparam RF_Physical.Address2 = 0;
    defparam RF_Physical.EnIntRXInterrupt = 1;
    defparam RF_Physical.EnIntTXInterrupt = 0;
    defparam RF_Physical.FlowControl = 0;
    defparam RF_Physical.HalfDuplexEn = 0;
    defparam RF_Physical.HwTXEnSignal = 0;
    defparam RF_Physical.NumDataBits = 8;
    defparam RF_Physical.NumStopBits = 1;
    defparam RF_Physical.ParityType = 0;
    defparam RF_Physical.RXEnable = 1;
    defparam RF_Physical.TXEnable = 1;

	wire [0:0] tmpOE__Rx_RF_net;
	wire [0:0] tmpIO_0__Rx_RF_net;
	wire [0:0] tmpINTERRUPT_0__Rx_RF_net;
	electrical [0:0] tmpSIOVREF__Rx_RF_net;

	cy_psoc3_pins_v1_10
		#(.id("1425177d-0d0e-4468-8bcc-e638e5509a9b"),
		  .drive_mode(3'b001),
		  .ibuf_enabled(1'b1),
		  .init_dr_st(1'b0),
		  .input_clk_en(0),
		  .input_sync(1'b1),
		  .input_sync_mode(1'b0),
		  .intr_mode(2'b00),
		  .invert_in_clock(0),
		  .invert_in_clock_en(0),
		  .invert_in_reset(0),
		  .invert_out_clock(0),
		  .invert_out_clock_en(0),
		  .invert_out_reset(0),
		  .io_voltage(""),
		  .layout_mode("CONTIGUOUS"),
		  .oe_conn(1'b0),
		  .oe_reset(0),
		  .oe_sync(1'b0),
		  .output_clk_en(0),
		  .output_clock_mode(1'b0),
		  .output_conn(1'b0),
		  .output_mode(1'b0),
		  .output_reset(0),
		  .output_sync(1'b0),
		  .pa_in_clock(-1),
		  .pa_in_clock_en(-1),
		  .pa_in_reset(-1),
		  .pa_out_clock(-1),
		  .pa_out_clock_en(-1),
		  .pa_out_reset(-1),
		  .pin_aliases(""),
		  .pin_mode("I"),
		  .por_state(4),
		  .sio_group_cnt(0),
		  .sio_hyst(1'b1),
		  .sio_ibuf(""),
		  .sio_info(2'b00),
		  .sio_obuf(""),
		  .sio_refsel(""),
		  .sio_vtrip(""),
		  .sio_hifreq(""),
		  .sio_vohsel(""),
		  .slew_rate(1'b0),
		  .spanning(0),
		  .use_annotation(1'b0),
		  .vtrip(2'b00),
		  .width(1),
		  .ovt_hyst_trim(1'b0),
		  .ovt_needed(1'b0),
		  .ovt_slew_control(2'b00),
		  .input_buffer_sel(2'b00))
		Rx_RF
		 (.oe(tmpOE__Rx_RF_net),
		  .y({1'b0}),
		  .fb({Net_7}),
		  .io({tmpIO_0__Rx_RF_net[0:0]}),
		  .siovref(tmpSIOVREF__Rx_RF_net),
		  .interrupt({tmpINTERRUPT_0__Rx_RF_net[0:0]}),
		  .in_clock({1'b0}),
		  .in_clock_en({1'b1}),
		  .in_reset({1'b0}),
		  .out_clock({1'b0}),
		  .out_clock_en({1'b1}),
		  .out_reset({1'b0}));

	assign tmpOE__Rx_RF_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{1'b1} : {1'b1};

	wire [0:0] tmpOE__Tx_RF_net;
	wire [0:0] tmpFB_0__Tx_RF_net;
	wire [0:0] tmpIO_0__Tx_RF_net;
	wire [0:0] tmpINTERRUPT_0__Tx_RF_net;
	electrical [0:0] tmpSIOVREF__Tx_RF_net;

	cy_psoc3_pins_v1_10
		#(.id("ed092b9b-d398-4703-be89-cebf998501f6"),
		  .drive_mode(3'b110),
		  .ibuf_enabled(1'b1),
		  .init_dr_st(1'b1),
		  .input_clk_en(0),
		  .input_sync(1'b1),
		  .input_sync_mode(1'b0),
		  .intr_mode(2'b00),
		  .invert_in_clock(0),
		  .invert_in_clock_en(0),
		  .invert_in_reset(0),
		  .invert_out_clock(0),
		  .invert_out_clock_en(0),
		  .invert_out_reset(0),
		  .io_voltage(""),
		  .layout_mode("CONTIGUOUS"),
		  .oe_conn(1'b0),
		  .oe_reset(0),
		  .oe_sync(1'b0),
		  .output_clk_en(0),
		  .output_clock_mode(1'b0),
		  .output_conn(1'b1),
		  .output_mode(1'b0),
		  .output_reset(0),
		  .output_sync(1'b0),
		  .pa_in_clock(-1),
		  .pa_in_clock_en(-1),
		  .pa_in_reset(-1),
		  .pa_out_clock(-1),
		  .pa_out_clock_en(-1),
		  .pa_out_reset(-1),
		  .pin_aliases(""),
		  .pin_mode("O"),
		  .por_state(4),
		  .sio_group_cnt(0),
		  .sio_hyst(1'b1),
		  .sio_ibuf(""),
		  .sio_info(2'b00),
		  .sio_obuf(""),
		  .sio_refsel(""),
		  .sio_vtrip(""),
		  .sio_hifreq(""),
		  .sio_vohsel(""),
		  .slew_rate(1'b0),
		  .spanning(0),
		  .use_annotation(1'b0),
		  .vtrip(2'b10),
		  .width(1),
		  .ovt_hyst_trim(1'b0),
		  .ovt_needed(1'b0),
		  .ovt_slew_control(2'b00),
		  .input_buffer_sel(2'b00))
		Tx_RF
		 (.oe(tmpOE__Tx_RF_net),
		  .y({Net_2}),
		  .fb({tmpFB_0__Tx_RF_net[0:0]}),
		  .io({tmpIO_0__Tx_RF_net[0:0]}),
		  .siovref(tmpSIOVREF__Tx_RF_net),
		  .interrupt({tmpINTERRUPT_0__Tx_RF_net[0:0]}),
		  .in_clock({1'b0}),
		  .in_clock_en({1'b1}),
		  .in_reset({1'b0}),
		  .out_clock({1'b0}),
		  .out_clock_en({1'b1}),
		  .out_reset({1'b0}));

	assign tmpOE__Tx_RF_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{1'b1} : {1'b1};

    CyControlReg_v1_80 Reset_DispenserTimer (
        .control_1(Net_859),
        .control_2(Net_860),
        .control_3(Net_861),
        .control_0(Net_12),
        .control_4(Net_862),
        .control_5(Net_863),
        .control_6(Net_864),
        .control_7(Net_865),
        .clock(1'b0),
        .reset(1'b0));
    defparam Reset_DispenserTimer.Bit0Mode = 0;
    defparam Reset_DispenserTimer.Bit1Mode = 0;
    defparam Reset_DispenserTimer.Bit2Mode = 0;
    defparam Reset_DispenserTimer.Bit3Mode = 0;
    defparam Reset_DispenserTimer.Bit4Mode = 0;
    defparam Reset_DispenserTimer.Bit5Mode = 0;
    defparam Reset_DispenserTimer.Bit6Mode = 0;
    defparam Reset_DispenserTimer.Bit7Mode = 0;
    defparam Reset_DispenserTimer.BitValue = 0;
    defparam Reset_DispenserTimer.BusDisplay = 0;
    defparam Reset_DispenserTimer.ExtrReset = 0;
    defparam Reset_DispenserTimer.NumOutputs = 1;


	cy_isr_v1_0
		#(.int_type(2'b10))
		DispenserStateTimeout_ISR
		 (.int_signal(Net_59));


    Timer_v2_70_1 Timer_DispenserState (
        .reset(Net_12),
        .interrupt(Net_59),
        .enable(1'b1),
        .trigger(1'b1),
        .capture(1'b0),
        .capture_out(Net_857),
        .tc(Net_858),
        .clock(Net_10));
    defparam Timer_DispenserState.CaptureCount = 2;
    defparam Timer_DispenserState.CaptureCounterEnabled = 0;
    defparam Timer_DispenserState.DeviceFamily = "PSoC5";
    defparam Timer_DispenserState.InterruptOnCapture = 0;
    defparam Timer_DispenserState.InterruptOnTC = 1;
    defparam Timer_DispenserState.Resolution = 16;
    defparam Timer_DispenserState.SiliconRevision = "0";

    ShiftReg_v2_30_2 ShiftReg_iButtonCRC4LSB (
        .shift_in(Net_87),
        .load(1'b0),
        .store(1'b0),
        .clock(Net_995),
        .reset(Net_160),
        .shift_out(Net_996),
        .interrupt(Net_162));
    defparam ShiftReg_iButtonCRC4LSB.Direction = 1;
    defparam ShiftReg_iButtonCRC4LSB.FifoSize = 4;
    defparam ShiftReg_iButtonCRC4LSB.Length = 4;

    CyControlReg_v1_80 Input_iButton (
        .control_1(Net_884),
        .control_2(Net_885),
        .control_3(Net_886),
        .control_0(Net_872),
        .control_4(Net_887),
        .control_5(Net_888),
        .control_6(Net_889),
        .control_7(Net_890),
        .clock(1'b0),
        .reset(1'b0));
    defparam Input_iButton.Bit0Mode = 0;
    defparam Input_iButton.Bit1Mode = 0;
    defparam Input_iButton.Bit2Mode = 0;
    defparam Input_iButton.Bit3Mode = 0;
    defparam Input_iButton.Bit4Mode = 0;
    defparam Input_iButton.Bit5Mode = 0;
    defparam Input_iButton.Bit6Mode = 0;
    defparam Input_iButton.Bit7Mode = 0;
    defparam Input_iButton.BitValue = 0;
    defparam Input_iButton.BusDisplay = 0;
    defparam Input_iButton.ExtrReset = 0;
    defparam Input_iButton.NumOutputs = 1;

    CyControlReg_v1_80 Clock_iButton (
        .control_1(Net_204),
        .control_2(Net_205),
        .control_3(Net_206),
        .control_0(Net_995),
        .control_4(Net_208),
        .control_5(Net_209),
        .control_6(Net_210),
        .control_7(Net_211),
        .clock(1'b0),
        .reset(1'b0));
    defparam Clock_iButton.Bit0Mode = 0;
    defparam Clock_iButton.Bit1Mode = 0;
    defparam Clock_iButton.Bit2Mode = 0;
    defparam Clock_iButton.Bit3Mode = 0;
    defparam Clock_iButton.Bit4Mode = 0;
    defparam Clock_iButton.Bit5Mode = 0;
    defparam Clock_iButton.Bit6Mode = 0;
    defparam Clock_iButton.Bit7Mode = 0;
    defparam Clock_iButton.BitValue = 0;
    defparam Clock_iButton.BusDisplay = 0;
    defparam Clock_iButton.ExtrReset = 0;
    defparam Clock_iButton.NumOutputs = 1;

    CRC_v2_40 CRC_Security (
        .di(Net_133),
        .clock(Net_111),
        .reset(Net_140),
        .enable(Net_113));
    defparam CRC_Security.Resolution = 8;
    defparam CRC_Security.TimeMultiplexing = 0;

    ShiftReg_v2_30_3 ShiftReg_Security (
        .shift_in(1'b0),
        .load(1'b0),
        .store(1'b0),
        .clock(Net_111),
        .reset(1'b0),
        .shift_out(Net_133),
        .interrupt(Net_166));
    defparam ShiftReg_Security.Direction = 1;
    defparam ShiftReg_Security.FifoSize = 4;
    defparam ShiftReg_Security.Length = 8;

    assign Net_113 = 1'h1;

    CyControlReg_v1_80 Clock_CRC (
        .control_1(Net_255),
        .control_2(Net_256),
        .control_3(Net_257),
        .control_0(Net_111),
        .control_4(Net_259),
        .control_5(Net_260),
        .control_6(Net_261),
        .control_7(Net_262),
        .clock(1'b0),
        .reset(1'b0));
    defparam Clock_CRC.Bit0Mode = 0;
    defparam Clock_CRC.Bit1Mode = 0;
    defparam Clock_CRC.Bit2Mode = 0;
    defparam Clock_CRC.Bit3Mode = 0;
    defparam Clock_CRC.Bit4Mode = 0;
    defparam Clock_CRC.Bit5Mode = 0;
    defparam Clock_CRC.Bit6Mode = 0;
    defparam Clock_CRC.Bit7Mode = 0;
    defparam Clock_CRC.BitValue = 0;
    defparam Clock_CRC.BusDisplay = 0;
    defparam Clock_CRC.ExtrReset = 0;
    defparam Clock_CRC.NumOutputs = 1;


    assign Net_152 = Net_996 ^ Net_997;

    CyControlReg_v1_80 Reset_CRC (
        .control_1(Net_287),
        .control_2(Net_288),
        .control_3(Net_289),
        .control_0(Net_140),
        .control_4(Net_290),
        .control_5(Net_291),
        .control_6(Net_292),
        .control_7(Net_293),
        .clock(1'b0),
        .reset(1'b0));
    defparam Reset_CRC.Bit0Mode = 0;
    defparam Reset_CRC.Bit1Mode = 0;
    defparam Reset_CRC.Bit2Mode = 0;
    defparam Reset_CRC.Bit3Mode = 0;
    defparam Reset_CRC.Bit4Mode = 0;
    defparam Reset_CRC.Bit5Mode = 0;
    defparam Reset_CRC.Bit6Mode = 0;
    defparam Reset_CRC.Bit7Mode = 0;
    defparam Reset_CRC.BitValue = 0;
    defparam Reset_CRC.BusDisplay = 0;
    defparam Reset_CRC.ExtrReset = 0;
    defparam Reset_CRC.NumOutputs = 1;


	cy_clock_v1_0
		#(.id("c0fb34bd-1044-4931-9788-16b01ce89812"),
		  .source_clock_id(""),
		  .divisor(0),
		  .period("1000000000"),
		  .is_direct(0),
		  .is_digital(1))
		timer_clock
		 (.clock_out(Net_10));


    assign Net_29 = 1'h0;

    CyControlReg_v1_80 Reset_iButton (
        .control_1(Net_327),
        .control_2(Net_328),
        .control_3(Net_329),
        .control_0(Net_160),
        .control_4(Net_331),
        .control_5(Net_332),
        .control_6(Net_333),
        .control_7(Net_334),
        .clock(1'b0),
        .reset(1'b0));
    defparam Reset_iButton.Bit0Mode = 0;
    defparam Reset_iButton.Bit1Mode = 0;
    defparam Reset_iButton.Bit2Mode = 0;
    defparam Reset_iButton.Bit3Mode = 0;
    defparam Reset_iButton.Bit4Mode = 0;
    defparam Reset_iButton.Bit5Mode = 0;
    defparam Reset_iButton.Bit6Mode = 0;
    defparam Reset_iButton.Bit7Mode = 0;
    defparam Reset_iButton.BitValue = 0;
    defparam Reset_iButton.BusDisplay = 0;
    defparam Reset_iButton.ExtrReset = 0;
    defparam Reset_iButton.NumOutputs = 1;


    assign Net_87 = Net_872 ^ Net_997;

    UART_v2_50_4 Printer (
        .cts_n(1'b0),
        .tx(Net_358),
        .rts_n(Net_468),
        .tx_en(Net_469),
        .clock(Net_470),
        .reset(Net_362),
        .rx(Net_363),
        .tx_interrupt(Net_471),
        .rx_interrupt(Net_472),
        .tx_data(Net_473),
        .tx_clk(Net_474),
        .rx_data(Net_475),
        .rx_clk(Net_476));
    defparam Printer.Address1 = 0;
    defparam Printer.Address2 = 0;
    defparam Printer.EnIntRXInterrupt = 0;
    defparam Printer.EnIntTXInterrupt = 0;
    defparam Printer.FlowControl = 0;
    defparam Printer.HalfDuplexEn = 0;
    defparam Printer.HwTXEnSignal = 0;
    defparam Printer.NumDataBits = 8;
    defparam Printer.NumStopBits = 1;
    defparam Printer.ParityType = 0;
    defparam Printer.RXEnable = 1;
    defparam Printer.TXEnable = 1;

	wire [0:0] tmpOE__Rx_Print_net;
	wire [0:0] tmpIO_0__Rx_Print_net;
	wire [0:0] tmpINTERRUPT_0__Rx_Print_net;
	electrical [0:0] tmpSIOVREF__Rx_Print_net;

	cy_psoc3_pins_v1_10
		#(.id("6cb233f3-9be5-4aa2-8a34-f7ec52d99db5"),
		  .drive_mode(3'b001),
		  .ibuf_enabled(1'b1),
		  .init_dr_st(1'b0),
		  .input_clk_en(0),
		  .input_sync(1'b1),
		  .input_sync_mode(1'b0),
		  .intr_mode(2'b00),
		  .invert_in_clock(0),
		  .invert_in_clock_en(0),
		  .invert_in_reset(0),
		  .invert_out_clock(0),
		  .invert_out_clock_en(0),
		  .invert_out_reset(0),
		  .io_voltage(""),
		  .layout_mode("CONTIGUOUS"),
		  .oe_conn(1'b0),
		  .oe_reset(0),
		  .oe_sync(1'b0),
		  .output_clk_en(0),
		  .output_clock_mode(1'b0),
		  .output_conn(1'b0),
		  .output_mode(1'b0),
		  .output_reset(0),
		  .output_sync(1'b0),
		  .pa_in_clock(-1),
		  .pa_in_clock_en(-1),
		  .pa_in_reset(-1),
		  .pa_out_clock(-1),
		  .pa_out_clock_en(-1),
		  .pa_out_reset(-1),
		  .pin_aliases(""),
		  .pin_mode("I"),
		  .por_state(4),
		  .sio_group_cnt(0),
		  .sio_hyst(1'b1),
		  .sio_ibuf(""),
		  .sio_info(2'b00),
		  .sio_obuf(""),
		  .sio_refsel(""),
		  .sio_vtrip(""),
		  .sio_hifreq(""),
		  .sio_vohsel(""),
		  .slew_rate(1'b0),
		  .spanning(0),
		  .use_annotation(1'b0),
		  .vtrip(2'b00),
		  .width(1),
		  .ovt_hyst_trim(1'b0),
		  .ovt_needed(1'b0),
		  .ovt_slew_control(2'b00),
		  .input_buffer_sel(2'b00))
		Rx_Print
		 (.oe(tmpOE__Rx_Print_net),
		  .y({1'b0}),
		  .fb({Net_363}),
		  .io({tmpIO_0__Rx_Print_net[0:0]}),
		  .siovref(tmpSIOVREF__Rx_Print_net),
		  .interrupt({tmpINTERRUPT_0__Rx_Print_net[0:0]}),
		  .in_clock({1'b0}),
		  .in_clock_en({1'b1}),
		  .in_reset({1'b0}),
		  .out_clock({1'b0}),
		  .out_clock_en({1'b1}),
		  .out_reset({1'b0}));

	assign tmpOE__Rx_Print_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{1'b1} : {1'b1};

	wire [0:0] tmpOE__Tx_Print_net;
	wire [0:0] tmpFB_0__Tx_Print_net;
	wire [0:0] tmpIO_0__Tx_Print_net;
	wire [0:0] tmpINTERRUPT_0__Tx_Print_net;
	electrical [0:0] tmpSIOVREF__Tx_Print_net;

	cy_psoc3_pins_v1_10
		#(.id("8c416670-f619-4293-9bef-04c89d57c95a"),
		  .drive_mode(3'b110),
		  .ibuf_enabled(1'b1),
		  .init_dr_st(1'b1),
		  .input_clk_en(0),
		  .input_sync(1'b1),
		  .input_sync_mode(1'b0),
		  .intr_mode(2'b00),
		  .invert_in_clock(0),
		  .invert_in_clock_en(0),
		  .invert_in_reset(0),
		  .invert_out_clock(0),
		  .invert_out_clock_en(0),
		  .invert_out_reset(0),
		  .io_voltage(""),
		  .layout_mode("CONTIGUOUS"),
		  .oe_conn(1'b0),
		  .oe_reset(0),
		  .oe_sync(1'b0),
		  .output_clk_en(0),
		  .output_clock_mode(1'b0),
		  .output_conn(1'b1),
		  .output_mode(1'b0),
		  .output_reset(0),
		  .output_sync(1'b0),
		  .pa_in_clock(-1),
		  .pa_in_clock_en(-1),
		  .pa_in_reset(-1),
		  .pa_out_clock(-1),
		  .pa_out_clock_en(-1),
		  .pa_out_reset(-1),
		  .pin_aliases(""),
		  .pin_mode("O"),
		  .por_state(4),
		  .sio_group_cnt(0),
		  .sio_hyst(1'b1),
		  .sio_ibuf(""),
		  .sio_info(2'b00),
		  .sio_obuf(""),
		  .sio_refsel(""),
		  .sio_vtrip(""),
		  .sio_hifreq(""),
		  .sio_vohsel(""),
		  .slew_rate(1'b0),
		  .spanning(0),
		  .use_annotation(1'b0),
		  .vtrip(2'b10),
		  .width(1),
		  .ovt_hyst_trim(1'b0),
		  .ovt_needed(1'b0),
		  .ovt_slew_control(2'b00),
		  .input_buffer_sel(2'b00))
		Tx_Print
		 (.oe(tmpOE__Tx_Print_net),
		  .y({Net_358}),
		  .fb({tmpFB_0__Tx_Print_net[0:0]}),
		  .io({tmpIO_0__Tx_Print_net[0:0]}),
		  .siovref(tmpSIOVREF__Tx_Print_net),
		  .interrupt({tmpINTERRUPT_0__Tx_Print_net[0:0]}),
		  .in_clock({1'b0}),
		  .in_clock_en({1'b1}),
		  .in_reset({1'b0}),
		  .out_clock({1'b0}),
		  .out_clock_en({1'b1}),
		  .out_reset({1'b0}));

	assign tmpOE__Tx_Print_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{1'b1} : {1'b1};

    assign Net_362 = 1'h0;

    UART_v2_50_5 Dispenser (
        .cts_n(1'b0),
        .tx(Net_392),
        .rts_n(Net_478),
        .tx_en(Net_479),
        .clock(Net_1067),
        .reset(Net_429),
        .rx(Net_397),
        .tx_interrupt(Net_480),
        .rx_interrupt(Net_481),
        .tx_data(Net_482),
        .tx_clk(Net_483),
        .rx_data(Net_484),
        .rx_clk(Net_485));
    defparam Dispenser.Address1 = 0;
    defparam Dispenser.Address2 = 0;
    defparam Dispenser.EnIntRXInterrupt = 1;
    defparam Dispenser.EnIntTXInterrupt = 0;
    defparam Dispenser.FlowControl = 0;
    defparam Dispenser.HalfDuplexEn = 0;
    defparam Dispenser.HwTXEnSignal = 0;
    defparam Dispenser.NumDataBits = 8;
    defparam Dispenser.NumStopBits = 1;
    defparam Dispenser.ParityType = 1;
    defparam Dispenser.RXEnable = 1;
    defparam Dispenser.TXEnable = 1;

	wire [0:0] tmpOE__Rx_Pump_net;
	wire [0:0] tmpIO_0__Rx_Pump_net;
	wire [0:0] tmpINTERRUPT_0__Rx_Pump_net;
	electrical [0:0] tmpSIOVREF__Rx_Pump_net;

	cy_psoc3_pins_v1_10
		#(.id("46dcc609-45dc-4cb1-aa6f-045ef9dc7369"),
		  .drive_mode(3'b001),
		  .ibuf_enabled(1'b1),
		  .init_dr_st(1'b0),
		  .input_clk_en(0),
		  .input_sync(1'b1),
		  .input_sync_mode(1'b0),
		  .intr_mode(2'b00),
		  .invert_in_clock(0),
		  .invert_in_clock_en(0),
		  .invert_in_reset(0),
		  .invert_out_clock(0),
		  .invert_out_clock_en(0),
		  .invert_out_reset(0),
		  .io_voltage(""),
		  .layout_mode("CONTIGUOUS"),
		  .oe_conn(1'b0),
		  .oe_reset(0),
		  .oe_sync(1'b0),
		  .output_clk_en(0),
		  .output_clock_mode(1'b0),
		  .output_conn(1'b0),
		  .output_mode(1'b0),
		  .output_reset(0),
		  .output_sync(1'b0),
		  .pa_in_clock(-1),
		  .pa_in_clock_en(-1),
		  .pa_in_reset(-1),
		  .pa_out_clock(-1),
		  .pa_out_clock_en(-1),
		  .pa_out_reset(-1),
		  .pin_aliases(""),
		  .pin_mode("I"),
		  .por_state(4),
		  .sio_group_cnt(0),
		  .sio_hyst(1'b1),
		  .sio_ibuf(""),
		  .sio_info(2'b00),
		  .sio_obuf(""),
		  .sio_refsel(""),
		  .sio_vtrip(""),
		  .sio_hifreq(""),
		  .sio_vohsel(""),
		  .slew_rate(1'b0),
		  .spanning(0),
		  .use_annotation(1'b0),
		  .vtrip(2'b00),
		  .width(1),
		  .ovt_hyst_trim(1'b0),
		  .ovt_needed(1'b0),
		  .ovt_slew_control(2'b00),
		  .input_buffer_sel(2'b00))
		Rx_Pump
		 (.oe(tmpOE__Rx_Pump_net),
		  .y({1'b0}),
		  .fb({Net_397}),
		  .io({tmpIO_0__Rx_Pump_net[0:0]}),
		  .siovref(tmpSIOVREF__Rx_Pump_net),
		  .interrupt({tmpINTERRUPT_0__Rx_Pump_net[0:0]}),
		  .in_clock({1'b0}),
		  .in_clock_en({1'b1}),
		  .in_reset({1'b0}),
		  .out_clock({1'b0}),
		  .out_clock_en({1'b1}),
		  .out_reset({1'b0}));

	assign tmpOE__Rx_Pump_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{1'b1} : {1'b1};

	wire [0:0] tmpOE__Tx_Pump_net;
	wire [0:0] tmpFB_0__Tx_Pump_net;
	wire [0:0] tmpIO_0__Tx_Pump_net;
	wire [0:0] tmpINTERRUPT_0__Tx_Pump_net;
	electrical [0:0] tmpSIOVREF__Tx_Pump_net;

	cy_psoc3_pins_v1_10
		#(.id("f7480020-7ef3-4b29-8029-376bf52cbbd1"),
		  .drive_mode(3'b110),
		  .ibuf_enabled(1'b1),
		  .init_dr_st(1'b1),
		  .input_clk_en(0),
		  .input_sync(1'b1),
		  .input_sync_mode(1'b0),
		  .intr_mode(2'b00),
		  .invert_in_clock(0),
		  .invert_in_clock_en(0),
		  .invert_in_reset(0),
		  .invert_out_clock(0),
		  .invert_out_clock_en(0),
		  .invert_out_reset(0),
		  .io_voltage(""),
		  .layout_mode("CONTIGUOUS"),
		  .oe_conn(1'b0),
		  .oe_reset(0),
		  .oe_sync(1'b0),
		  .output_clk_en(0),
		  .output_clock_mode(1'b0),
		  .output_conn(1'b1),
		  .output_mode(1'b0),
		  .output_reset(0),
		  .output_sync(1'b0),
		  .pa_in_clock(-1),
		  .pa_in_clock_en(-1),
		  .pa_in_reset(-1),
		  .pa_out_clock(-1),
		  .pa_out_clock_en(-1),
		  .pa_out_reset(-1),
		  .pin_aliases(""),
		  .pin_mode("O"),
		  .por_state(4),
		  .sio_group_cnt(0),
		  .sio_hyst(1'b1),
		  .sio_ibuf(""),
		  .sio_info(2'b00),
		  .sio_obuf(""),
		  .sio_refsel(""),
		  .sio_vtrip(""),
		  .sio_hifreq(""),
		  .sio_vohsel(""),
		  .slew_rate(1'b0),
		  .spanning(0),
		  .use_annotation(1'b0),
		  .vtrip(2'b10),
		  .width(1),
		  .ovt_hyst_trim(1'b0),
		  .ovt_needed(1'b0),
		  .ovt_slew_control(2'b00),
		  .input_buffer_sel(2'b00))
		Tx_Pump
		 (.oe(tmpOE__Tx_Pump_net),
		  .y({Net_392}),
		  .fb({tmpFB_0__Tx_Pump_net[0:0]}),
		  .io({tmpIO_0__Tx_Pump_net[0:0]}),
		  .siovref(tmpSIOVREF__Tx_Pump_net),
		  .interrupt({tmpINTERRUPT_0__Tx_Pump_net[0:0]}),
		  .in_clock({1'b0}),
		  .in_clock_en({1'b1}),
		  .in_reset({1'b0}),
		  .out_clock({1'b0}),
		  .out_clock_en({1'b1}),
		  .out_reset({1'b0}));

	assign tmpOE__Tx_Pump_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{1'b1} : {1'b1};


	cy_clock_v1_0
		#(.id("cf030d93-def8-4211-a913-f85a706bad09"),
		  .source_clock_id(""),
		  .divisor(0),
		  .period("21691973969.6312"),
		  .is_direct(0),
		  .is_digital(1))
		Clock_Pump
		 (.clock_out(Net_1067));


    assign Net_429 = 1'h0;

    UART_v2_50_6 Display1 (
        .cts_n(1'b0),
        .tx(Net_444),
        .rts_n(Net_487),
        .tx_en(Net_488),
        .clock(1'b0),
        .reset(Net_490),
        .rx(Net_449),
        .tx_interrupt(Net_491),
        .rx_interrupt(Net_492),
        .tx_data(Net_493),
        .tx_clk(Net_494),
        .rx_data(Net_495),
        .rx_clk(Net_496));
    defparam Display1.Address1 = 0;
    defparam Display1.Address2 = 0;
    defparam Display1.EnIntRXInterrupt = 1;
    defparam Display1.EnIntTXInterrupt = 0;
    defparam Display1.FlowControl = 0;
    defparam Display1.HalfDuplexEn = 0;
    defparam Display1.HwTXEnSignal = 0;
    defparam Display1.NumDataBits = 8;
    defparam Display1.NumStopBits = 1;
    defparam Display1.ParityType = 0;
    defparam Display1.RXEnable = 1;
    defparam Display1.TXEnable = 1;

	wire [0:0] tmpOE__Rx_Disp1_net;
	wire [0:0] tmpIO_0__Rx_Disp1_net;
	wire [0:0] tmpINTERRUPT_0__Rx_Disp1_net;
	electrical [0:0] tmpSIOVREF__Rx_Disp1_net;

	cy_psoc3_pins_v1_10
		#(.id("5f053855-6ffc-41a9-ad9d-d02fe9e6834c"),
		  .drive_mode(3'b001),
		  .ibuf_enabled(1'b1),
		  .init_dr_st(1'b0),
		  .input_clk_en(0),
		  .input_sync(1'b1),
		  .input_sync_mode(1'b0),
		  .intr_mode(2'b00),
		  .invert_in_clock(0),
		  .invert_in_clock_en(0),
		  .invert_in_reset(0),
		  .invert_out_clock(0),
		  .invert_out_clock_en(0),
		  .invert_out_reset(0),
		  .io_voltage(""),
		  .layout_mode("CONTIGUOUS"),
		  .oe_conn(1'b0),
		  .oe_reset(0),
		  .oe_sync(1'b0),
		  .output_clk_en(0),
		  .output_clock_mode(1'b0),
		  .output_conn(1'b0),
		  .output_mode(1'b0),
		  .output_reset(0),
		  .output_sync(1'b0),
		  .pa_in_clock(-1),
		  .pa_in_clock_en(-1),
		  .pa_in_reset(-1),
		  .pa_out_clock(-1),
		  .pa_out_clock_en(-1),
		  .pa_out_reset(-1),
		  .pin_aliases(""),
		  .pin_mode("I"),
		  .por_state(4),
		  .sio_group_cnt(0),
		  .sio_hyst(1'b1),
		  .sio_ibuf(""),
		  .sio_info(2'b00),
		  .sio_obuf(""),
		  .sio_refsel(""),
		  .sio_vtrip(""),
		  .sio_hifreq(""),
		  .sio_vohsel(""),
		  .slew_rate(1'b0),
		  .spanning(0),
		  .use_annotation(1'b0),
		  .vtrip(2'b00),
		  .width(1),
		  .ovt_hyst_trim(1'b0),
		  .ovt_needed(1'b0),
		  .ovt_slew_control(2'b00),
		  .input_buffer_sel(2'b00))
		Rx_Disp1
		 (.oe(tmpOE__Rx_Disp1_net),
		  .y({1'b0}),
		  .fb({Net_449}),
		  .io({tmpIO_0__Rx_Disp1_net[0:0]}),
		  .siovref(tmpSIOVREF__Rx_Disp1_net),
		  .interrupt({tmpINTERRUPT_0__Rx_Disp1_net[0:0]}),
		  .in_clock({1'b0}),
		  .in_clock_en({1'b1}),
		  .in_reset({1'b0}),
		  .out_clock({1'b0}),
		  .out_clock_en({1'b1}),
		  .out_reset({1'b0}));

	assign tmpOE__Rx_Disp1_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{1'b1} : {1'b1};

	wire [0:0] tmpOE__Tx_Disp1_net;
	wire [0:0] tmpFB_0__Tx_Disp1_net;
	wire [0:0] tmpIO_0__Tx_Disp1_net;
	wire [0:0] tmpINTERRUPT_0__Tx_Disp1_net;
	electrical [0:0] tmpSIOVREF__Tx_Disp1_net;

	cy_psoc3_pins_v1_10
		#(.id("da9ffc36-684c-4ddd-b8f6-dc39e0b176eb"),
		  .drive_mode(3'b110),
		  .ibuf_enabled(1'b1),
		  .init_dr_st(1'b1),
		  .input_clk_en(0),
		  .input_sync(1'b1),
		  .input_sync_mode(1'b0),
		  .intr_mode(2'b00),
		  .invert_in_clock(0),
		  .invert_in_clock_en(0),
		  .invert_in_reset(0),
		  .invert_out_clock(0),
		  .invert_out_clock_en(0),
		  .invert_out_reset(0),
		  .io_voltage(""),
		  .layout_mode("CONTIGUOUS"),
		  .oe_conn(1'b0),
		  .oe_reset(0),
		  .oe_sync(1'b0),
		  .output_clk_en(0),
		  .output_clock_mode(1'b0),
		  .output_conn(1'b1),
		  .output_mode(1'b0),
		  .output_reset(0),
		  .output_sync(1'b0),
		  .pa_in_clock(-1),
		  .pa_in_clock_en(-1),
		  .pa_in_reset(-1),
		  .pa_out_clock(-1),
		  .pa_out_clock_en(-1),
		  .pa_out_reset(-1),
		  .pin_aliases(""),
		  .pin_mode("O"),
		  .por_state(4),
		  .sio_group_cnt(0),
		  .sio_hyst(1'b1),
		  .sio_ibuf(""),
		  .sio_info(2'b00),
		  .sio_obuf(""),
		  .sio_refsel(""),
		  .sio_vtrip(""),
		  .sio_hifreq(""),
		  .sio_vohsel(""),
		  .slew_rate(1'b0),
		  .spanning(0),
		  .use_annotation(1'b0),
		  .vtrip(2'b10),
		  .width(1),
		  .ovt_hyst_trim(1'b0),
		  .ovt_needed(1'b0),
		  .ovt_slew_control(2'b00),
		  .input_buffer_sel(2'b00))
		Tx_Disp1
		 (.oe(tmpOE__Tx_Disp1_net),
		  .y({Net_444}),
		  .fb({tmpFB_0__Tx_Disp1_net[0:0]}),
		  .io({tmpIO_0__Tx_Disp1_net[0:0]}),
		  .siovref(tmpSIOVREF__Tx_Disp1_net),
		  .interrupt({tmpINTERRUPT_0__Tx_Disp1_net[0:0]}),
		  .in_clock({1'b0}),
		  .in_clock_en({1'b1}),
		  .in_reset({1'b0}),
		  .out_clock({1'b0}),
		  .out_clock_en({1'b1}),
		  .out_reset({1'b0}));

	assign tmpOE__Tx_Disp1_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{1'b1} : {1'b1};

    assign Net_490 = 1'h0;

    UART_v2_50_7 Display2 (
        .cts_n(1'b0),
        .tx(Net_499),
        .rts_n(Net_523),
        .tx_en(Net_524),
        .clock(1'b0),
        .reset(Net_526),
        .rx(Net_504),
        .tx_interrupt(Net_527),
        .rx_interrupt(Net_528),
        .tx_data(Net_529),
        .tx_clk(Net_530),
        .rx_data(Net_531),
        .rx_clk(Net_532));
    defparam Display2.Address1 = 0;
    defparam Display2.Address2 = 0;
    defparam Display2.EnIntRXInterrupt = 1;
    defparam Display2.EnIntTXInterrupt = 0;
    defparam Display2.FlowControl = 0;
    defparam Display2.HalfDuplexEn = 0;
    defparam Display2.HwTXEnSignal = 0;
    defparam Display2.NumDataBits = 8;
    defparam Display2.NumStopBits = 1;
    defparam Display2.ParityType = 0;
    defparam Display2.RXEnable = 1;
    defparam Display2.TXEnable = 1;

	wire [0:0] tmpOE__Rx_Disp2_net;
	wire [0:0] tmpIO_0__Rx_Disp2_net;
	wire [0:0] tmpINTERRUPT_0__Rx_Disp2_net;
	electrical [0:0] tmpSIOVREF__Rx_Disp2_net;

	cy_psoc3_pins_v1_10
		#(.id("b161f1cb-237d-40e8-ba9b-e2a4cb6ef0c1"),
		  .drive_mode(3'b001),
		  .ibuf_enabled(1'b1),
		  .init_dr_st(1'b0),
		  .input_clk_en(0),
		  .input_sync(1'b1),
		  .input_sync_mode(1'b0),
		  .intr_mode(2'b00),
		  .invert_in_clock(0),
		  .invert_in_clock_en(0),
		  .invert_in_reset(0),
		  .invert_out_clock(0),
		  .invert_out_clock_en(0),
		  .invert_out_reset(0),
		  .io_voltage(""),
		  .layout_mode("CONTIGUOUS"),
		  .oe_conn(1'b0),
		  .oe_reset(0),
		  .oe_sync(1'b0),
		  .output_clk_en(0),
		  .output_clock_mode(1'b0),
		  .output_conn(1'b0),
		  .output_mode(1'b0),
		  .output_reset(0),
		  .output_sync(1'b0),
		  .pa_in_clock(-1),
		  .pa_in_clock_en(-1),
		  .pa_in_reset(-1),
		  .pa_out_clock(-1),
		  .pa_out_clock_en(-1),
		  .pa_out_reset(-1),
		  .pin_aliases(""),
		  .pin_mode("I"),
		  .por_state(4),
		  .sio_group_cnt(0),
		  .sio_hyst(1'b1),
		  .sio_ibuf(""),
		  .sio_info(2'b00),
		  .sio_obuf(""),
		  .sio_refsel(""),
		  .sio_vtrip(""),
		  .sio_hifreq(""),
		  .sio_vohsel(""),
		  .slew_rate(1'b0),
		  .spanning(0),
		  .use_annotation(1'b0),
		  .vtrip(2'b00),
		  .width(1),
		  .ovt_hyst_trim(1'b0),
		  .ovt_needed(1'b0),
		  .ovt_slew_control(2'b00),
		  .input_buffer_sel(2'b00))
		Rx_Disp2
		 (.oe(tmpOE__Rx_Disp2_net),
		  .y({1'b0}),
		  .fb({Net_504}),
		  .io({tmpIO_0__Rx_Disp2_net[0:0]}),
		  .siovref(tmpSIOVREF__Rx_Disp2_net),
		  .interrupt({tmpINTERRUPT_0__Rx_Disp2_net[0:0]}),
		  .in_clock({1'b0}),
		  .in_clock_en({1'b1}),
		  .in_reset({1'b0}),
		  .out_clock({1'b0}),
		  .out_clock_en({1'b1}),
		  .out_reset({1'b0}));

	assign tmpOE__Rx_Disp2_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{1'b1} : {1'b1};

	wire [0:0] tmpOE__Tx_Disp2_net;
	wire [0:0] tmpFB_0__Tx_Disp2_net;
	wire [0:0] tmpIO_0__Tx_Disp2_net;
	wire [0:0] tmpINTERRUPT_0__Tx_Disp2_net;
	electrical [0:0] tmpSIOVREF__Tx_Disp2_net;

	cy_psoc3_pins_v1_10
		#(.id("c3580982-d808-4fc2-86d1-05be1b2449ad"),
		  .drive_mode(3'b110),
		  .ibuf_enabled(1'b1),
		  .init_dr_st(1'b1),
		  .input_clk_en(0),
		  .input_sync(1'b1),
		  .input_sync_mode(1'b0),
		  .intr_mode(2'b00),
		  .invert_in_clock(0),
		  .invert_in_clock_en(0),
		  .invert_in_reset(0),
		  .invert_out_clock(0),
		  .invert_out_clock_en(0),
		  .invert_out_reset(0),
		  .io_voltage(""),
		  .layout_mode("CONTIGUOUS"),
		  .oe_conn(1'b0),
		  .oe_reset(0),
		  .oe_sync(1'b0),
		  .output_clk_en(0),
		  .output_clock_mode(1'b0),
		  .output_conn(1'b1),
		  .output_mode(1'b0),
		  .output_reset(0),
		  .output_sync(1'b0),
		  .pa_in_clock(-1),
		  .pa_in_clock_en(-1),
		  .pa_in_reset(-1),
		  .pa_out_clock(-1),
		  .pa_out_clock_en(-1),
		  .pa_out_reset(-1),
		  .pin_aliases(""),
		  .pin_mode("O"),
		  .por_state(4),
		  .sio_group_cnt(0),
		  .sio_hyst(1'b1),
		  .sio_ibuf(""),
		  .sio_info(2'b00),
		  .sio_obuf(""),
		  .sio_refsel(""),
		  .sio_vtrip(""),
		  .sio_hifreq(""),
		  .sio_vohsel(""),
		  .slew_rate(1'b0),
		  .spanning(0),
		  .use_annotation(1'b0),
		  .vtrip(2'b10),
		  .width(1),
		  .ovt_hyst_trim(1'b0),
		  .ovt_needed(1'b0),
		  .ovt_slew_control(2'b00),
		  .input_buffer_sel(2'b00))
		Tx_Disp2
		 (.oe(tmpOE__Tx_Disp2_net),
		  .y({Net_499}),
		  .fb({tmpFB_0__Tx_Disp2_net[0:0]}),
		  .io({tmpIO_0__Tx_Disp2_net[0:0]}),
		  .siovref(tmpSIOVREF__Tx_Disp2_net),
		  .interrupt({tmpINTERRUPT_0__Tx_Disp2_net[0:0]}),
		  .in_clock({1'b0}),
		  .in_clock_en({1'b1}),
		  .in_reset({1'b0}),
		  .out_clock({1'b0}),
		  .out_clock_en({1'b1}),
		  .out_reset({1'b0}));

	assign tmpOE__Tx_Disp2_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{1'b1} : {1'b1};

    assign Net_526 = 1'h0;


	cy_isr_v1_0
		#(.int_type(2'b10))
		Counter_ISR
		 (.int_signal(Net_701));


	wire [0:0] tmpOE__SDA_Bus_net;
	wire [0:0] tmpFB_0__SDA_Bus_net;
	wire [0:0] tmpINTERRUPT_0__SDA_Bus_net;
	electrical [0:0] tmpSIOVREF__SDA_Bus_net;

	cy_psoc3_pins_v1_10
		#(.id("22863ebe-a37b-476f-b252-6e49a8c00b12"),
		  .drive_mode(3'b100),
		  .ibuf_enabled(1'b1),
		  .init_dr_st(1'b1),
		  .input_clk_en(0),
		  .input_sync(1'b1),
		  .input_sync_mode(1'b0),
		  .intr_mode(2'b00),
		  .invert_in_clock(0),
		  .invert_in_clock_en(0),
		  .invert_in_reset(0),
		  .invert_out_clock(0),
		  .invert_out_clock_en(0),
		  .invert_out_reset(0),
		  .io_voltage(""),
		  .layout_mode("CONTIGUOUS"),
		  .oe_conn(1'b0),
		  .oe_reset(0),
		  .oe_sync(1'b0),
		  .output_clk_en(0),
		  .output_clock_mode(1'b0),
		  .output_conn(1'b0),
		  .output_mode(1'b0),
		  .output_reset(0),
		  .output_sync(1'b0),
		  .pa_in_clock(-1),
		  .pa_in_clock_en(-1),
		  .pa_in_reset(-1),
		  .pa_out_clock(-1),
		  .pa_out_clock_en(-1),
		  .pa_out_reset(-1),
		  .pin_aliases(""),
		  .pin_mode("B"),
		  .por_state(4),
		  .sio_group_cnt(0),
		  .sio_hyst(1'b1),
		  .sio_ibuf(""),
		  .sio_info(2'b00),
		  .sio_obuf(""),
		  .sio_refsel(""),
		  .sio_vtrip(""),
		  .sio_hifreq(""),
		  .sio_vohsel(""),
		  .slew_rate(1'b0),
		  .spanning(0),
		  .use_annotation(1'b0),
		  .vtrip(2'b10),
		  .width(1),
		  .ovt_hyst_trim(1'b0),
		  .ovt_needed(1'b0),
		  .ovt_slew_control(2'b00),
		  .input_buffer_sel(2'b00))
		SDA_Bus
		 (.oe(tmpOE__SDA_Bus_net),
		  .y({1'b0}),
		  .fb({tmpFB_0__SDA_Bus_net[0:0]}),
		  .io({Net_625}),
		  .siovref(tmpSIOVREF__SDA_Bus_net),
		  .interrupt({tmpINTERRUPT_0__SDA_Bus_net[0:0]}),
		  .in_clock({1'b0}),
		  .in_clock_en({1'b1}),
		  .in_reset({1'b0}),
		  .out_clock({1'b0}),
		  .out_clock_en({1'b1}),
		  .out_reset({1'b0}));

	assign tmpOE__SDA_Bus_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{1'b1} : {1'b1};

	wire [0:0] tmpOE__SCL_Bus_net;
	wire [0:0] tmpFB_0__SCL_Bus_net;
	wire [0:0] tmpINTERRUPT_0__SCL_Bus_net;
	electrical [0:0] tmpSIOVREF__SCL_Bus_net;

	cy_psoc3_pins_v1_10
		#(.id("02f2cf2c-2c7a-49df-9246-7a3435c21be3"),
		  .drive_mode(3'b100),
		  .ibuf_enabled(1'b1),
		  .init_dr_st(1'b1),
		  .input_clk_en(0),
		  .input_sync(1'b1),
		  .input_sync_mode(1'b0),
		  .intr_mode(2'b00),
		  .invert_in_clock(0),
		  .invert_in_clock_en(0),
		  .invert_in_reset(0),
		  .invert_out_clock(0),
		  .invert_out_clock_en(0),
		  .invert_out_reset(0),
		  .io_voltage(""),
		  .layout_mode("CONTIGUOUS"),
		  .oe_conn(1'b0),
		  .oe_reset(0),
		  .oe_sync(1'b0),
		  .output_clk_en(0),
		  .output_clock_mode(1'b0),
		  .output_conn(1'b0),
		  .output_mode(1'b0),
		  .output_reset(0),
		  .output_sync(1'b0),
		  .pa_in_clock(-1),
		  .pa_in_clock_en(-1),
		  .pa_in_reset(-1),
		  .pa_out_clock(-1),
		  .pa_out_clock_en(-1),
		  .pa_out_reset(-1),
		  .pin_aliases(""),
		  .pin_mode("B"),
		  .por_state(4),
		  .sio_group_cnt(0),
		  .sio_hyst(1'b1),
		  .sio_ibuf(""),
		  .sio_info(2'b00),
		  .sio_obuf(""),
		  .sio_refsel(""),
		  .sio_vtrip(""),
		  .sio_hifreq(""),
		  .sio_vohsel(""),
		  .slew_rate(1'b0),
		  .spanning(0),
		  .use_annotation(1'b0),
		  .vtrip(2'b10),
		  .width(1),
		  .ovt_hyst_trim(1'b0),
		  .ovt_needed(1'b0),
		  .ovt_slew_control(2'b00),
		  .input_buffer_sel(2'b00))
		SCL_Bus
		 (.oe(tmpOE__SCL_Bus_net),
		  .y({1'b0}),
		  .fb({tmpFB_0__SCL_Bus_net[0:0]}),
		  .io({Net_626}),
		  .siovref(tmpSIOVREF__SCL_Bus_net),
		  .interrupt({tmpINTERRUPT_0__SCL_Bus_net[0:0]}),
		  .in_clock({1'b0}),
		  .in_clock_en({1'b1}),
		  .in_reset({1'b0}),
		  .out_clock({1'b0}),
		  .out_clock_en({1'b1}),
		  .out_reset({1'b0}));

	assign tmpOE__SCL_Bus_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{1'b1} : {1'b1};

    I2C_v3_50_8 I2C_Bus (
        .sda(Net_625),
        .scl(Net_626),
        .clock(1'b0),
        .reset(1'b0),
        .bclk(Net_658),
        .iclk(Net_659),
        .scl_i(1'b0),
        .sda_i(1'b0),
        .scl_o(Net_662),
        .sda_o(Net_663),
        .itclk(Net_664));

	wire [0:0] tmpOE__iButton1_net;
	wire [0:0] tmpFB_0__iButton1_net;
	wire [0:0] tmpINTERRUPT_0__iButton1_net;
	electrical [0:0] tmpSIOVREF__iButton1_net;

	cy_psoc3_pins_v1_10
		#(.id("8d318d8b-cf7b-4b6b-b02c-ab1c5c49d0ba"),
		  .drive_mode(3'b010),
		  .ibuf_enabled(1'b1),
		  .init_dr_st(1'b1),
		  .input_clk_en(0),
		  .input_sync(1'b1),
		  .input_sync_mode(1'b0),
		  .intr_mode(2'b00),
		  .invert_in_clock(0),
		  .invert_in_clock_en(0),
		  .invert_in_reset(0),
		  .invert_out_clock(0),
		  .invert_out_clock_en(0),
		  .invert_out_reset(0),
		  .io_voltage(""),
		  .layout_mode("CONTIGUOUS"),
		  .oe_conn(1'b0),
		  .oe_reset(0),
		  .oe_sync(1'b0),
		  .output_clk_en(0),
		  .output_clock_mode(1'b0),
		  .output_conn(1'b0),
		  .output_mode(1'b0),
		  .output_reset(0),
		  .output_sync(1'b0),
		  .pa_in_clock(-1),
		  .pa_in_clock_en(-1),
		  .pa_in_reset(-1),
		  .pa_out_clock(-1),
		  .pa_out_clock_en(-1),
		  .pa_out_reset(-1),
		  .pin_aliases(""),
		  .pin_mode("B"),
		  .por_state(4),
		  .sio_group_cnt(0),
		  .sio_hyst(1'b1),
		  .sio_ibuf(""),
		  .sio_info(2'b00),
		  .sio_obuf(""),
		  .sio_refsel(""),
		  .sio_vtrip(""),
		  .sio_hifreq(""),
		  .sio_vohsel(""),
		  .slew_rate(1'b0),
		  .spanning(0),
		  .use_annotation(1'b0),
		  .vtrip(2'b00),
		  .width(1),
		  .ovt_hyst_trim(1'b0),
		  .ovt_needed(1'b0),
		  .ovt_slew_control(2'b00),
		  .input_buffer_sel(2'b00))
		iButton1
		 (.oe(tmpOE__iButton1_net),
		  .y({1'b0}),
		  .fb({tmpFB_0__iButton1_net[0:0]}),
		  .io({Net_1007}),
		  .siovref(tmpSIOVREF__iButton1_net),
		  .interrupt({tmpINTERRUPT_0__iButton1_net[0:0]}),
		  .in_clock({1'b0}),
		  .in_clock_en({1'b1}),
		  .in_reset({1'b0}),
		  .out_clock({1'b0}),
		  .out_clock_en({1'b1}),
		  .out_reset({1'b0}));

	assign tmpOE__iButton1_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{1'b1} : {1'b1};

	wire [0:0] tmpOE__iButton2_net;
	wire [0:0] tmpFB_0__iButton2_net;
	wire [0:0] tmpINTERRUPT_0__iButton2_net;
	electrical [0:0] tmpSIOVREF__iButton2_net;

	cy_psoc3_pins_v1_10
		#(.id("f6efac75-b064-41ae-8698-535ee05f14bb"),
		  .drive_mode(3'b010),
		  .ibuf_enabled(1'b1),
		  .init_dr_st(1'b1),
		  .input_clk_en(0),
		  .input_sync(1'b1),
		  .input_sync_mode(1'b0),
		  .intr_mode(2'b00),
		  .invert_in_clock(0),
		  .invert_in_clock_en(0),
		  .invert_in_reset(0),
		  .invert_out_clock(0),
		  .invert_out_clock_en(0),
		  .invert_out_reset(0),
		  .io_voltage(""),
		  .layout_mode("CONTIGUOUS"),
		  .oe_conn(1'b0),
		  .oe_reset(0),
		  .oe_sync(1'b0),
		  .output_clk_en(0),
		  .output_clock_mode(1'b0),
		  .output_conn(1'b0),
		  .output_mode(1'b0),
		  .output_reset(0),
		  .output_sync(1'b0),
		  .pa_in_clock(-1),
		  .pa_in_clock_en(-1),
		  .pa_in_reset(-1),
		  .pa_out_clock(-1),
		  .pa_out_clock_en(-1),
		  .pa_out_reset(-1),
		  .pin_aliases(""),
		  .pin_mode("B"),
		  .por_state(4),
		  .sio_group_cnt(0),
		  .sio_hyst(1'b1),
		  .sio_ibuf(""),
		  .sio_info(2'b00),
		  .sio_obuf(""),
		  .sio_refsel(""),
		  .sio_vtrip(""),
		  .sio_hifreq(""),
		  .sio_vohsel(""),
		  .slew_rate(1'b0),
		  .spanning(0),
		  .use_annotation(1'b0),
		  .vtrip(2'b00),
		  .width(1),
		  .ovt_hyst_trim(1'b0),
		  .ovt_needed(1'b0),
		  .ovt_slew_control(2'b00),
		  .input_buffer_sel(2'b00))
		iButton2
		 (.oe(tmpOE__iButton2_net),
		  .y({1'b0}),
		  .fb({tmpFB_0__iButton2_net[0:0]}),
		  .io({Net_1008}),
		  .siovref(tmpSIOVREF__iButton2_net),
		  .interrupt({tmpINTERRUPT_0__iButton2_net[0:0]}),
		  .in_clock({1'b0}),
		  .in_clock_en({1'b1}),
		  .in_reset({1'b0}),
		  .out_clock({1'b0}),
		  .out_clock_en({1'b1}),
		  .out_reset({1'b0}));

	assign tmpOE__iButton2_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{1'b1} : {1'b1};

	wire [0:0] tmpOE__eepromWP_net;
	wire [0:0] tmpFB_0__eepromWP_net;
	wire [0:0] tmpIO_0__eepromWP_net;
	wire [0:0] tmpINTERRUPT_0__eepromWP_net;
	electrical [0:0] tmpSIOVREF__eepromWP_net;

	cy_psoc3_pins_v1_10
		#(.id("e851a3b9-efb8-48be-bbb8-b303b216c393"),
		  .drive_mode(3'b110),
		  .ibuf_enabled(1'b1),
		  .init_dr_st(1'b1),
		  .input_clk_en(0),
		  .input_sync(1'b1),
		  .input_sync_mode(1'b0),
		  .intr_mode(2'b00),
		  .invert_in_clock(0),
		  .invert_in_clock_en(0),
		  .invert_in_reset(0),
		  .invert_out_clock(0),
		  .invert_out_clock_en(0),
		  .invert_out_reset(0),
		  .io_voltage(""),
		  .layout_mode("CONTIGUOUS"),
		  .oe_conn(1'b0),
		  .oe_reset(0),
		  .oe_sync(1'b0),
		  .output_clk_en(0),
		  .output_clock_mode(1'b0),
		  .output_conn(1'b0),
		  .output_mode(1'b0),
		  .output_reset(0),
		  .output_sync(1'b0),
		  .pa_in_clock(-1),
		  .pa_in_clock_en(-1),
		  .pa_in_reset(-1),
		  .pa_out_clock(-1),
		  .pa_out_clock_en(-1),
		  .pa_out_reset(-1),
		  .pin_aliases(""),
		  .pin_mode("O"),
		  .por_state(4),
		  .sio_group_cnt(0),
		  .sio_hyst(1'b1),
		  .sio_ibuf(""),
		  .sio_info(2'b00),
		  .sio_obuf(""),
		  .sio_refsel(""),
		  .sio_vtrip(""),
		  .sio_hifreq(""),
		  .sio_vohsel(""),
		  .slew_rate(1'b0),
		  .spanning(0),
		  .use_annotation(1'b0),
		  .vtrip(2'b10),
		  .width(1),
		  .ovt_hyst_trim(1'b0),
		  .ovt_needed(1'b0),
		  .ovt_slew_control(2'b00),
		  .input_buffer_sel(2'b00))
		eepromWP
		 (.oe(tmpOE__eepromWP_net),
		  .y({1'b0}),
		  .fb({tmpFB_0__eepromWP_net[0:0]}),
		  .io({tmpIO_0__eepromWP_net[0:0]}),
		  .siovref(tmpSIOVREF__eepromWP_net),
		  .interrupt({tmpINTERRUPT_0__eepromWP_net[0:0]}),
		  .in_clock({1'b0}),
		  .in_clock_en({1'b1}),
		  .in_reset({1'b0}),
		  .out_clock({1'b0}),
		  .out_clock_en({1'b1}),
		  .out_reset({1'b0}));

	assign tmpOE__eepromWP_net = (`CYDEV_CHIP_MEMBER_USED == `CYDEV_CHIP_MEMBER_3A && `CYDEV_CHIP_REVISION_USED < `CYDEV_CHIP_REVISION_3A_ES3) ? ~{1'b1} : {1'b1};


	cy_clock_v1_0
		#(.id("bca5ea02-cf0a-4b57-ae17-0a67aa363f2d"),
		  .source_clock_id(""),
		  .divisor(0),
		  .period("1000000000"),
		  .is_direct(0),
		  .is_digital(1))
		timer_clock_1
		 (.clock_out(Net_734));


    ZeroTerminal ZeroTerminal_1 (
        .z(Net_736));

    Timer_v2_70_9 Timer_Tick (
        .reset(Net_736),
        .interrupt(Net_701),
        .enable(1'b1),
        .trigger(1'b1),
        .capture(1'b0),
        .capture_out(Net_815),
        .tc(Net_816),
        .clock(Net_734));
    defparam Timer_Tick.CaptureCount = 2;
    defparam Timer_Tick.CaptureCounterEnabled = 0;
    defparam Timer_Tick.DeviceFamily = "PSoC5";
    defparam Timer_Tick.InterruptOnCapture = 0;
    defparam Timer_Tick.InterruptOnTC = 1;
    defparam Timer_Tick.Resolution = 16;
    defparam Timer_Tick.SiliconRevision = "0";


    assign Net_950 = Net_152 ^ Net_997;

    ShiftReg_v2_30_10 ShiftReg_iButtonCRC3MSB (
        .shift_in(Net_950),
        .load(1'b0),
        .store(1'b0),
        .clock(Net_995),
        .reset(Net_160),
        .shift_out(Net_997),
        .interrupt(Net_1004));
    defparam ShiftReg_iButtonCRC3MSB.Direction = 1;
    defparam ShiftReg_iButtonCRC3MSB.FifoSize = 4;
    defparam ShiftReg_iButtonCRC3MSB.Length = 3;

    CyStatusReg_v1_90 StatusReg_iButton5thCRCBit (
        .status_0(Net_152),
        .status_1(1'b0),
        .status_2(1'b0),
        .status_3(1'b0),
        .clock(Net_995),
        .status_4(1'b0),
        .status_5(1'b0),
        .status_6(1'b0),
        .status_7(1'b0),
        .intr(Net_991),
        .status_bus(8'b0));
    defparam StatusReg_iButton5thCRCBit.Bit0Mode = 0;
    defparam StatusReg_iButton5thCRCBit.Bit1Mode = 0;
    defparam StatusReg_iButton5thCRCBit.Bit2Mode = 0;
    defparam StatusReg_iButton5thCRCBit.Bit3Mode = 0;
    defparam StatusReg_iButton5thCRCBit.Bit4Mode = 0;
    defparam StatusReg_iButton5thCRCBit.Bit5Mode = 0;
    defparam StatusReg_iButton5thCRCBit.Bit6Mode = 0;
    defparam StatusReg_iButton5thCRCBit.Bit7Mode = 0;
    defparam StatusReg_iButton5thCRCBit.BusDisplay = 0;
    defparam StatusReg_iButton5thCRCBit.Interrupt = 0;
    defparam StatusReg_iButton5thCRCBit.MaskValue = 0;
    defparam StatusReg_iButton5thCRCBit.NumInputs = 1;


	cy_clock_v1_0
		#(.id("87512c90-b42b-4d83-9760-db815e37dd3d"),
		  .source_clock_id(""),
		  .divisor(0),
		  .period("13020833333.3333"),
		  .is_direct(0),
		  .is_digital(1))
		Clock_Printer
		 (.clock_out(Net_470));




endmodule

