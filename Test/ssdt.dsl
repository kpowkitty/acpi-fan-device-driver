/*
 * Intel ACPI Component Architecture
 * AML/ASL+ Disassembler version 20200925 (64-bit version)
 * Copyright (c) 2000 - 2020 Intel Corporation
 * 
 * Disassembling to symbolic ASL+ operators
 *
 * Disassembly of ssdt.bin, Sun May 11 15:09:37 2025
 *
 * Original Table Header:
 *     Signature        "SSDT"
 *     Length           0x000006F4 (1780)
 *     Revision         0x02
 *     Checksum         0x4D
 *     OEM ID           "VIRTIO"
 *     OEM Table ID     "FAKEFANS"
 *     OEM Revision     0x00000000 (0)
 *     Compiler ID      "INTL"
 *     Compiler Version 0x20241212 (539234834)
 */
DefinitionBlock ("", "SSDT", 2, "VIRTIO", "FAKEFANS", 0x00000000)
{
    Scope (\_SB)
    {
        Device (FAN0)
        {
            Name (_HID, EisaId ("PNP0C0B") /* Fan (Thermal Solution) */)  // _HID: Hardware ID
            Name (_UID, "VirtualFan0")  // _UID: Unique ID
            Name (FSTA, Zero)
            Name (FLVL, Zero)
            Name (PKG0, Package (0x04)
            {
                Zero, 
                Zero, 
                Zero, 
                Zero
            })
            Name (FPS0, Package (0x05)
            {
                Zero, 
                Zero, 
                Zero, 
                Zero, 
                Zero
            })
            Name (FPS1, Package (0x05)
            {
                One, 
                0x19, 
                0x01F4, 
                0x37, 
                0x0258
            })
            Name (FPS2, Package (0x05)
            {
                0x02, 
                0x32, 
                0x03E8, 
                0x6E, 
                0x04B0
            })
            Name (FPS3, Package (0x05)
            {
                0x03, 
                0x4B, 
                0x05DC, 
                0xA5, 
                0x0708
            })
            Name (FPS4, Package (0x05)
            {
                0x04, 
                0x64, 
                0x07D0, 
                0xDC, 
                0x0960
            })
            Name (FPSP, Package (0x06)
            {
                Zero, 
                FPS0, 
                FPS1, 
                FPS2, 
                FPS3, 
                FPS4
            })
            Method (_STA, 0, NotSerialized)  // _STA: Status
            {
                Return (0x0F)
            }

            Method (_FIF, 0, NotSerialized)  // _FIF: Fan Information
            {
                Return (PKG0) /* \_SB_.FAN0.PKG0 */
            }

            Method (_PSC, 0, NotSerialized)  // _PSC: Power State Current
            {
                Return (FSTA) /* \_SB_.FAN0.FSTA */
            }

            Method (_ON, 0, NotSerialized)  // _ON_: Power On
            {
                FSTA = One
            }

            Method (_OFF, 0, NotSerialized)  // _OFF: Power Off
            {
                FSTA = Zero
            }

            Method (_FSL, 1, NotSerialized)  // _FSL: Fan Set Level
            {
                If ((Arg0 > 0x03))
                {
                    FLVL = 0xFF
                }

                FLVL = Arg0
            }

            Method (_FST, 0, NotSerialized)  // _FST: Fan Status
            {
                If ((FLVL > 0x04))
                {
                    Return (Package (0x03)
                    {
                        0xFF, 
                        0xFF, 
                        0xFFFF
                    })
                }

                Local0 = Zero
                Local1 = Zero
                Local2 = Package (0x05)
                    {
                        Zero, 
                        Zero, 
                        Zero, 
                        Zero, 
                        Zero
                    }
                Local3 = Package (0x03)
                    {
                        Zero, 
                        Zero, 
                        Zero
                    }
                If ((FLVL == Zero))
                {
                    Local2 = FPS0 /* \_SB_.FAN0.FPS0 */
                }

                If ((FLVL == One))
                {
                    Local2 = FPS1 /* \_SB_.FAN0.FPS1 */
                }

                If ((FLVL == 0x02))
                {
                    Local2 = FPS2 /* \_SB_.FAN0.FPS2 */
                }

                If ((FLVL == 0x03))
                {
                    Local2 = FPS3 /* \_SB_.FAN0.FPS3 */
                }

                If ((FLVL == 0x04))
                {
                    Local2 = FPS4 /* \_SB_.FAN0.FPS4 */
                }

                Local0 = DerefOf (Local2 [Zero])
                Local1 = DerefOf (Local2 [0x02])
                Local3 [One] = Local0
                Local3 [0x02] = Local1
                Return (Local3)
            }

            Method (_FPS, 0, NotSerialized)  // _FPS: Fan Performance States
            {
                Return (FPSP) /* \_SB_.FAN0.FPSP */
            }
        }

        Device (FAN1)
        {
            Name (_HID, EisaId ("PNP0C0B") /* Fan (Thermal Solution) */)  // _HID: Hardware ID
            Name (_UID, "VirtualFan1")  // _UID: Unique ID
            Name (FSTA, Zero)
            Name (FLVL, Zero)
            Name (PKG1, Package (0x04)
            {
                Zero, 
                Zero, 
                Zero, 
                Zero
            })
            Name (FPS0, Package (0x05)
            {
                Zero, 
                Zero, 
                Zero, 
                Zero, 
                Zero
            })
            Name (FPS1, Package (0x05)
            {
                One, 
                0x19, 
                0x01F4, 
                0x37, 
                0x0258
            })
            Name (FPS2, Package (0x05)
            {
                0x02, 
                0x32, 
                0x03E8, 
                0x6E, 
                0x04B0
            })
            Name (FPS3, Package (0x05)
            {
                0x03, 
                0x4B, 
                0x05DC, 
                0xA5, 
                0x0708
            })
            Name (FPS4, Package (0x05)
            {
                0x04, 
                0x64, 
                0x07D0, 
                0xDC, 
                0x0960
            })
            Name (FPSP, Package (0x06)
            {
                Zero, 
                FPS0, 
                FPS1, 
                FPS2, 
                FPS3, 
                FPS4
            })
            Method (_STA, 0, NotSerialized)  // _STA: Status
            {
                Return (0x0F)
            }

            Method (_FIF, 0, NotSerialized)  // _FIF: Fan Information
            {
                Return (PKG1) /* \_SB_.FAN1.PKG1 */
            }

            Method (_PSC, 0, NotSerialized)  // _PSC: Power State Current
            {
                Return (FSTA) /* \_SB_.FAN1.FSTA */
            }

            Method (_ON, 0, NotSerialized)  // _ON_: Power On
            {
                FSTA = One
            }

            Method (_OFF, 0, NotSerialized)  // _OFF: Power Off
            {
                FSTA = Zero
            }

            Method (_FSL, 1, NotSerialized)  // _FSL: Fan Set Level
            {
                If ((Arg0 > 0x03))
                {
                    FLVL = 0xFF
                }

                FLVL = Arg0
            }

            Method (_FST, 0, NotSerialized)  // _FST: Fan Status
            {
                If ((FLVL > 0x04))
                {
                    Return (Package (0x03)
                    {
                        0xFF, 
                        0xFF, 
                        0xFFFF
                    })
                }

                Local0 = Zero
                Local1 = Zero
                Local2 = Package (0x05)
                    {
                        Zero, 
                        Zero, 
                        Zero, 
                        Zero, 
                        Zero
                    }
                Local3 = Package (0x03)
                    {
                        Zero, 
                        Zero, 
                        Zero
                    }
                If ((FLVL == Zero))
                {
                    Local2 = FPS0 /* \_SB_.FAN1.FPS0 */
                }

                If ((FLVL == One))
                {
                    Local2 = FPS1 /* \_SB_.FAN1.FPS1 */
                }

                If ((FLVL == 0x02))
                {
                    Local2 = FPS2 /* \_SB_.FAN1.FPS2 */
                }

                If ((FLVL == 0x03))
                {
                    Local2 = FPS3 /* \_SB_.FAN1.FPS3 */
                }

                If ((FLVL == 0x04))
                {
                    Local2 = FPS4 /* \_SB_.FAN1.FPS4 */
                }

                Local0 = DerefOf (Local2 [Zero])
                Local1 = DerefOf (Local2 [0x02])
                Local3 [One] = Local0
                Local3 [0x02] = Local1
                Return (Local3)
            }

            Method (_FPS, 0, NotSerialized)  // _FPS: Fan Performance States
            {
                Return (FPSP) /* \_SB_.FAN1.FPSP */
            }
        }

        Device (FAN2)
        {
            Name (_HID, EisaId ("PNP0C0B") /* Fan (Thermal Solution) */)  // _HID: Hardware ID
            Name (_UID, "VirtualFan2")  // _UID: Unique ID
            Name (FSTA, Zero)
            Name (FLVL, Zero)
            Name (PKG2, Package (0x04)
            {
                Zero, 
                Zero, 
                Zero, 
                Zero
            })
            Name (FPS0, Package (0x05)
            {
                Zero, 
                Zero, 
                Zero, 
                Zero, 
                Zero
            })
            Name (FPS1, Package (0x05)
            {
                One, 
                0x19, 
                0x01F4, 
                0x37, 
                0x0258
            })
            Name (FPS2, Package (0x05)
            {
                0x02, 
                0x32, 
                0x03E8, 
                0x6E, 
                0x04B0
            })
            Name (FPS3, Package (0x05)
            {
                0x03, 
                0x4B, 
                0x05DC, 
                0xA5, 
                0x0708
            })
            Name (FPS4, Package (0x05)
            {
                0x04, 
                0x64, 
                0x07D0, 
                0xDC, 
                0x0960
            })
            Name (FPSP, Package (0x06)
            {
                Zero, 
                FPS0, 
                FPS1, 
                FPS2, 
                FPS3, 
                FPS4
            })
            Method (_STA, 0, NotSerialized)  // _STA: Status
            {
                Return (0x0F)
            }

            Method (_FIF, 0, NotSerialized)  // _FIF: Fan Information
            {
                Return (PKG2) /* \_SB_.FAN2.PKG2 */
            }

            Method (_PSC, 0, NotSerialized)  // _PSC: Power State Current
            {
                Return (FSTA) /* \_SB_.FAN2.FSTA */
            }

            Method (_ON, 0, NotSerialized)  // _ON_: Power On
            {
                FSTA = One
            }

            Method (_OFF, 0, NotSerialized)  // _OFF: Power Off
            {
                FSTA = Zero
            }

            Method (_FSL, 1, NotSerialized)  // _FSL: Fan Set Level
            {
                If ((Arg0 > 0x03))
                {
                    FLVL = 0xFF
                }

                FLVL = Arg0
            }

            Method (_FST, 0, NotSerialized)  // _FST: Fan Status
            {
                If ((FLVL > 0x04))
                {
                    Return (Package (0x03)
                    {
                        0xFF, 
                        0xFF, 
                        0xFFFF
                    })
                }

                Local0 = Zero
                Local1 = Zero
                Local2 = Package (0x05)
                    {
                        Zero, 
                        Zero, 
                        Zero, 
                        Zero, 
                        Zero
                    }
                Local3 = Package (0x03)
                    {
                        Zero, 
                        Zero, 
                        Zero
                    }
                If ((FLVL == Zero))
                {
                    Local2 = FPS0 /* \_SB_.FAN2.FPS0 */
                }

                If ((FLVL == One))
                {
                    Local2 = FPS1 /* \_SB_.FAN2.FPS1 */
                }

                If ((FLVL == 0x02))
                {
                    Local2 = FPS2 /* \_SB_.FAN2.FPS2 */
                }

                If ((FLVL == 0x03))
                {
                    Local2 = FPS3 /* \_SB_.FAN2.FPS3 */
                }

                If ((FLVL == 0x04))
                {
                    Local2 = FPS4 /* \_SB_.FAN2.FPS4 */
                }

                Local0 = DerefOf (Local2 [Zero])
                Local1 = DerefOf (Local2 [0x02])
                Local3 [One] = Local0
                Local3 [0x02] = Local1
                Return (Local3)
            }

            Method (_FPS, 0, NotSerialized)  // _FPS: Fan Performance States
            {
                Return (FPSP) /* \_SB_.FAN2.FPSP */
            }
        }

        Device (FAN3)
        {
            Name (_HID, EisaId ("PNP0C0B") /* Fan (Thermal Solution) */)  // _HID: Hardware ID
            Name (_UID, "VirtualFan3")  // _UID: Unique ID
            Name (FSTA, Zero)
            Name (FLVL, Zero)
            Name (PKG3, Package (0x04)
            {
                Zero, 
                Zero, 
                Zero, 
                Zero
            })
            Name (FPS0, Package (0x05)
            {
                Zero, 
                Zero, 
                Zero, 
                Zero, 
                Zero
            })
            Name (FPS1, Package (0x05)
            {
                One, 
                0x19, 
                0x01F4, 
                0x37, 
                0x0258
            })
            Name (FPS2, Package (0x05)
            {
                0x02, 
                0x32, 
                0x03E8, 
                0x6E, 
                0x04B0
            })
            Name (FPS3, Package (0x05)
            {
                0x03, 
                0x4B, 
                0x05DC, 
                0xA5, 
                0x0708
            })
            Name (FPS4, Package (0x05)
            {
                0x04, 
                0x64, 
                0x07D0, 
                0xDC, 
                0x0960
            })
            Name (FPSP, Package (0x06)
            {
                Zero, 
                FPS0, 
                FPS1, 
                FPS2, 
                FPS3, 
                FPS4
            })
            Method (_STA, 0, NotSerialized)  // _STA: Status
            {
                Return (0x0F)
            }

            Method (_FIF, 0, NotSerialized)  // _FIF: Fan Information
            {
                Return (PKG3) /* \_SB_.FAN3.PKG3 */
            }

            Method (_PSC, 0, NotSerialized)  // _PSC: Power State Current
            {
                Return (FSTA) /* \_SB_.FAN3.FSTA */
            }

            Method (_ON, 0, NotSerialized)  // _ON_: Power On
            {
                FSTA = One
            }

            Method (_OFF, 0, NotSerialized)  // _OFF: Power Off
            {
                FSTA = Zero
            }

            Method (_FSL, 1, NotSerialized)  // _FSL: Fan Set Level
            {
                If ((Arg0 > 0x03))
                {
                    FLVL = 0xFF
                }

                FLVL = Arg0
            }

            Method (_FST, 0, NotSerialized)  // _FST: Fan Status
            {
                If ((FLVL > 0x04))
                {
                    Return (Package (0x03)
                    {
                        0xFF, 
                        0xFF, 
                        0xFFFF
                    })
                }

                Local0 = Zero
                Local1 = Zero
                Local2 = Package (0x05)
                    {
                        Zero, 
                        Zero, 
                        Zero, 
                        Zero, 
                        Zero
                    }
                Local3 = Package (0x03)
                    {
                        Zero, 
                        Zero, 
                        Zero
                    }
                If ((FLVL == Zero))
                {
                    Local2 = FPS0 /* \_SB_.FAN3.FPS0 */
                }

                If ((FLVL == One))
                {
                    Local2 = FPS1 /* \_SB_.FAN3.FPS1 */
                }

                If ((FLVL == 0x02))
                {
                    Local2 = FPS2 /* \_SB_.FAN3.FPS2 */
                }

                If ((FLVL == 0x03))
                {
                    Local2 = FPS3 /* \_SB_.FAN3.FPS3 */
                }

                If ((FLVL == 0x04))
                {
                    Local2 = FPS4 /* \_SB_.FAN3.FPS4 */
                }

                Local0 = DerefOf (Local2 [Zero])
                Local1 = DerefOf (Local2 [0x02])
                Local3 [One] = Local0
                Local3 [0x02] = Local1
                Return (Local3)
            }

            Method (_FPS, 0, NotSerialized)  // _FPS: Fan Performance States
            {
                Return (FPSP) /* \_SB_.FAN3.FPSP */
            }
        }
    }
}

