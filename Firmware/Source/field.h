#ifndef __FIELD_H				/* Avoid multiple inclusions */
#define __FIELD_H

#define	FLD_SRGVLTx		(11)
#define	FLD_SRGVLTy		(2)
#define	FLD_SRGDLYx		(30)
#define	FLD_SRGDLYy		(2)
#define	FLD_SRGNETx		(12)
#define	FLD_SRGNETy		(0)
#define	FLD_SRGWAVx		(20)
#define	FLD_SRGWAVy		(0)
#define	FLD_SRGOUTx		(12)
#define	FLD_SRGOUTy		(1)
#define	FLD_SRGDTAx		(28)
#define	FLD_SRGDTAy		(1)

#define	FLD_SRGCL1x		(15)
#define	FLD_SRGCL1y		(5)
#define	FLD_SRGCL2x		(21)
#define	FLD_SRGCL2y		(5)
#define	FLD_SRGCL3x		(27)
#define	FLD_SRGCL3y		(5)
#define	FLD_SRGCNUx0	(32)
#define	FLD_SRGCNUy0	(5)
#define FLD_SRGCNUx1	(33)
#define FLD_SRGCNUy1	(5)
#define	FLD_SRGCPEx0	(38)
#define	FLD_SRGCPEy0	(5)
#define	FLD_SRGCPEx1	(27)
#define	FLD_SRGCPEy1	(5)

#define	FLD_SRGSRCx		(12)
#define	FLD_SRGSRCy		(4)
#define	FLD_SRGEUTx		(30)
#define	FLD_SRGEUTy		(4)
#define	FLD_SRGLSMx		(12)
#define	FLD_SRGLSMy		(3)
#define	FLD_SRGANGx		(30)
#define	FLD_SRGANGy		(3)
#define	FLD_SRGPKSx		(0)
#define	FLD_SRGPKSy		(6)

#define	FLD_PKSRCx		(20)
#define	FLD_PKSRCy		(2)
#define	FLD_PKVHIx		(20)
#define	FLD_PKVHIy		(3)
#define	FLD_PKVLOx		(25)
#define	FLD_PKVLOy		(3)
#define	FLD_PKICHx		(20)
#define	FLD_PKICHy		(4)

#define	FLD_RMSPKLx		(10)
#define	FLD_RMSPKLy		(6)
#define	FLD_RMSMAXx		(10)
#define	FLD_RMSMAXy		(7)
#define	FLD_RMSMINx		(30)
#define	FLD_RMSMINy		(7)

#define	FLD_EFTPKLx		(10)
#define	FLD_EFTPKLy		(3)
#define	FLD_EFTMAXx		(10)
#define	FLD_EFTMAXy		(4)
#define	FLD_EFTMINx		(10)
#define	FLD_EFTMINy		(5)

#define	FLD_CALNETx		(14)
#define	FLD_CALNETy		(1)
#define	FLD_CALWAVx		(22)
#define	FLD_CALWAVy		(1)
#define	FLD_CALK5Dx		(34)
#define	FLD_CALK5Dy		(2)
#define	FLD_CALVFPx		(14)
#define	FLD_CALVFPy		(2)
#define	FLD_CALCPLx		(14)
#define	FLD_CALCPLy		(3)
#define	FLD_CALIO1x		(14)
#define	FLD_CALIO1y		(4)
#define	FLD_CALIO2x		(34)
#define	FLD_CALIO2y		(4)
#define	FLD_CALIO3x		(14)
#define	FLD_CALIO3y		(5)
#define	FLD_CALIO4x		(34)
#define	FLD_CALIO4y		(5)
#define	FLD_CALIO5x		(14)
#define	FLD_CALIO5y		(6)

#define	FLD_EFTBOXx		(11)
#define	FLD_EFTBOXy		(0)
#define	FLD_EFTOUTx		(11)
#define	FLD_EFTOUTy		(1)
#define	FLD_EFTMDEx		(26)
#define	FLD_EFTMDEy		(0)
#define	FLD_EFTWAVx		(26)
#define	FLD_EFTWAVy		(1)
#define	FLD_EFTVLTx		(10)
#define	FLD_EFTVLTy		(2)
#define	FLD_EFTFRQx		(26)
#define	FLD_EFTFRQy		(2)
#define	FLD_EFTUNTx		(31)
#define	FLD_EFTUNTy		(2)
#define	FLD_EFTDURx		(11)
#define	FLD_EFTDURy		(3)
#define	FLD_EFTPERx		(26)
#define	FLD_EFTPERy		(3)
#define	FLD_EFTLSMx		(11)
#define	FLD_EFTLSMy		(4)
#define	FLD_EFTANGx		(26)
#define	FLD_EFTANGy		(4)
#define	FLD_EFTSRCx		(11)
#define	FLD_EFTSRCy		(5)
#define	FLD_EFTEUTx		(26)
#define	FLD_EFTEUTy		(5)
#define	FLD_EFTCL1x		(38)
#define	FLD_EFTCL1y		(1)
#define	FLD_EFTCL2x		(38)
#define	FLD_EFTCL2y		(2)
#define	FLD_EFTCL3x		(38)
#define	FLD_EFTCL3y		(3)
#define	FLD_EFTCNUx		(38)
#define	FLD_EFTCNUy		(4)
#define	FLD_EFTCPEx0	(38)
#define	FLD_EFTCPEy0	(5)
#define	FLD_EFTCPEx1	(38)
#define	FLD_EFTCPEy1	(3)
#define	FLD_EFTPKSx		(0)
#define	FLD_EFTPKSy		(6)
#define	FLD_EFTINFOx	(31)
#define	FLD_EFTINFOy	(0)

#define	FLD_CALEFTx		    (13)
#define	FLD_CALEFTy		    (1)
#define	FLD_CAL_CLAMP0250Vx	(20)
#define	FLD_CAL_CLAMP0250Vy	(2)
#define	FLD_CAL_CLAMP4400Vx	(34)
#define	FLD_CAL_CLAMP4400Vy	(2)
#define	FLD_CAL_CLAMP4401Vx	(20)
#define	FLD_CAL_CLAMP4401Vy	(3)
#define	FLD_CAL_CLAMP8000Vx	(34)
#define	FLD_CAL_CLAMP8000Vy	(3)
#define	FLD_CAL_CPL_0250Vx	(20)
#define	FLD_CAL_CPL_0250Vy	(4)
#define	FLD_CAL_CPL_4400Vx	(34)
#define	FLD_CAL_CPL_4400Vy	(4)
#define	FLD_CAL_CPL_4401Vx	(20)
#define	FLD_CAL_CPL_4401Vy	(5)
#define	FLD_CAL_CPL_8000Vx	(34)
#define	FLD_CAL_CPL_8000Vy	(5)

#endif								/* ifndef FIELD_H */
